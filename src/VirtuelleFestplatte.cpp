// VirtuelleFestplatte.cpp
#include "VirtuelleFestplatte.h"
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

#define PROGNAME "passthrough"
#define ALLOCATION_UNIT 4096
#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))
#define info(format, ...) FspServiceLog(EVENTLOG_INFORMATION_TYPE, (PWSTR)format, __VA_ARGS__)
#define warn(format, ...) FspServiceLog(EVENTLOG_WARNING_TYPE, (PWSTR)format, __VA_ARGS__)
#define fail(format, ...) FspServiceLog(EVENTLOG_ERROR_TYPE, (PWSTR)format, __VA_ARGS__)
#define ConcatPath(Ptfs, FN, FP) (0 == StringCbPrintfW(FP, sizeof FP, L"%s%s", Ptfs->Path, FN))
#define HandleFromContext(FC) (((PTFS_FILE_CONTEXT *)(FC))->Handle)

// init static fur WinFSP
static VirtuelleFestplatte *vhdd;
static NTSTATUS staticStart(FSP_SERVICE *Service, ULONG argc, PWSTR *argv) {
    return vhdd->SvcStart(Service, argc, argv);
}
static NTSTATUS staticStop(FSP_SERVICE *Service) {
    return vhdd->SvcStop(Service);
}
static NTSTATUS staticGetVolumeInfo(FSP_FILE_SYSTEM *FileSystem, FSP_FSCTL_VOLUME_INFO *VolumeInfo) {
    return vhdd->GetVolumeInfo(FileSystem, VolumeInfo);
}
static NTSTATUS staticSetVolumeLabel_(FSP_FILE_SYSTEM *FileSystem, PWSTR VolumeLabel, FSP_FSCTL_VOLUME_INFO *VolumeInfo) {
    return vhdd->SetVolumeLabel_(FileSystem, VolumeLabel, VolumeInfo);
}
static NTSTATUS staticGetSecurityByName(FSP_FILE_SYSTEM *FileSystem,
                                        PWSTR FileName, PUINT32 PFileAttributes,
                                        PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize) {
    return vhdd->GetSecurityByName(FileSystem, FileName, PFileAttributes, SecurityDescriptor, PSecurityDescriptorSize);
}
static NTSTATUS staticCreate(FSP_FILE_SYSTEM *FileSystem,
                             PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess,
                             UINT32 FileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, UINT64 AllocationSize,
                             PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->Create(FileSystem, FileName, CreateOptions, GrantedAccess, FileAttributes, SecurityDescriptor,
                        AllocationSize, PFileContext, FileInfo);
}
static NTSTATUS staticOpen(FSP_FILE_SYSTEM *FileSystem,
                           PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess,
                           PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->Open(FileSystem, FileName, CreateOptions, GrantedAccess,
                      PFileContext, FileInfo);
}
static NTSTATUS staticOverwrite(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT32 FileAttributes, BOOLEAN ReplaceFileAttributes,
                                UINT64 AllocationSize, FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->Overwrite(FileSystem, FileContext, FileAttributes, ReplaceFileAttributes, AllocationSize, FileInfo);
}
static VOID staticCleanup(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, ULONG Flags) {
    return vhdd->Cleanup(FileSystem, FileContext, FileName, Flags);
}
static VOID staticClose(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext0) {
    return vhdd->Close(FileSystem, FileContext0);
}
static NTSTATUS staticRead(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PVOID Buffer, UINT64 Offset,
                           ULONG Length, PULONG PBytesTransferred) {
    return vhdd->Read(FileSystem, FileContext, Buffer, Offset, Length, PBytesTransferred);
}
static NTSTATUS staticWrite(FSP_FILE_SYSTEM *FileSystem,
                            PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length,
                            BOOLEAN WriteToEndOfFile, BOOLEAN ConstrainedIo,
                            PULONG PBytesTransferred, FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->Write(FileSystem, FileContext, Buffer, Offset, Length, WriteToEndOfFile, ConstrainedIo, PBytesTransferred, FileInfo);
}
static NTSTATUS staticFlush(FSP_FILE_SYSTEM *FileSystem,
                            PVOID FileContext,
                            FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->Flush(FileSystem, FileContext, FileInfo);
}
static NTSTATUS staticGetFileInfo(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->GetFileInfo(FileSystem, FileContext, FileInfo);
}
static NTSTATUS staticSetBasicInfo(FSP_FILE_SYSTEM *FileSystem,
                                   PVOID FileContext, UINT32 FileAttributes,
                                   UINT64 CreationTime, UINT64 LastAccessTime, UINT64 LastWriteTime, UINT64 ChangeTime,
                                   FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->SetBasicInfo(FileSystem, FileContext, FileAttributes, CreationTime, LastAccessTime, LastWriteTime, ChangeTime, FileInfo);
}
static NTSTATUS staticSetFileSize(FSP_FILE_SYSTEM *FileSystem,
                                  PVOID FileContext, UINT64 NewSize, BOOLEAN SetAllocationSize,
                                  FSP_FSCTL_FILE_INFO *FileInfo) {
    return vhdd->SetFileSize(FileSystem, FileContext, NewSize, SetAllocationSize, FileInfo);
}
static NTSTATUS staticRename(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext,
                             PWSTR FileName, PWSTR NewFileName, BOOLEAN ReplaceIfExists) {
    return vhdd->Rename(FileSystem, FileContext, FileName, NewFileName, ReplaceIfExists);
}
static NTSTATUS staticGetSecurity(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext,
                                  PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize) {
    return vhdd->GetSecurity(FileSystem, FileContext, SecurityDescriptor, PSecurityDescriptorSize);
}
static NTSTATUS staticSetSecurity(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext,
                                  SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR ModificationDescriptor) {
    return vhdd->SetSecurity(FileSystem, FileContext, SecurityInformation, ModificationDescriptor);
}
static NTSTATUS staticReadDirectory(FSP_FILE_SYSTEM *FileSystem,
                                    PVOID FileContext0, PWSTR Pattern, PWSTR Marker,
                                    PVOID Buffer, ULONG BufferLength, PULONG PBytesTransferred) {
    return vhdd->ReadDirectory(FileSystem, FileContext0, Pattern, Marker, Buffer, BufferLength, PBytesTransferred);
}
static NTSTATUS staticSetDelete(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, BOOLEAN DeleteFile) {
    return vhdd->SetDelete(FileSystem, FileContext, FileName, DeleteFile);
}

// VirtuelleFestplatte.cpp
VirtuelleFestplatte::VirtuelleFestplatte(std::wstring orginalVolume, std::wstring neuesVolume, CacheInterface &cache)
    : orginalVolume(orginalVolume), neuesVolume(neuesVolume), cache(cache) {
    vhdd = this;
    PtfsInterface = {
        .GetVolumeInfo = staticGetVolumeInfo,
        .SetVolumeLabel = staticSetVolumeLabel_,
        .GetSecurityByName = staticGetSecurityByName,
        .Create = staticCreate,
        .Open = staticOpen,
        .Overwrite = staticOverwrite,
        .Cleanup = staticCleanup,
        .Close = staticClose,
        .Read = staticRead,
        .Write = staticWrite,
        .Flush = staticFlush,
        .GetFileInfo = staticGetFileInfo,
        .SetBasicInfo = staticSetBasicInfo,
        .SetFileSize = staticSetFileSize,
        .Rename = staticRename,
        .GetSecurity = staticGetSecurity,
        .SetSecurity = staticSetSecurity,
        .ReadDirectory = staticReadDirectory,
        .SetDelete = staticSetDelete,
    };
}

void VirtuelleFestplatte::start() {
    if (!NT_SUCCESS(FspLoad(0))) {
        std::cout << "Fehler beim starten der vhdd." << std::endl;
        return;
    }
    PWSTR ServiceName = PWSTR(L"" PROGNAME);
    NTSTATUS status = FspServiceRun(ServiceName, staticStart, staticStop, 0);
    std::cout << "FspServiceRun status = " << (int)status << std::endl;
}

void VirtuelleFestplatte::stop() {}

// private Funktion:
bool VirtuelleFestplatte::isFail(NTSTATUS result, PTFS *ptfs) {
    if (!NT_SUCCESS(result)) {
        fail(L"isFail: VirtuelleFestplatte konte nicht erstellt werden.");
        if (0 != ptfs) {
            PtfsDelete(ptfs);
        }
        return true;
    }
    return false;
}

NTSTATUS VirtuelleFestplatte::SvcStop(FSP_SERVICE *Service) {
    PTFS *Ptfs = (PTFS *)Service->UserContext;
    FspFileSystemStopDispatcher(Ptfs->FileSystem);
    PtfsDelete(Ptfs);
    cache.Clear();
    return STATUS_SUCCESS;
}

VOID VirtuelleFestplatte::PtfsDelete(PTFS *Ptfs) {
    if (0 != Ptfs->FileSystem) {
        FspFileSystemDelete(Ptfs->FileSystem);
    }

    if (0 != Ptfs->Path) {
        free(Ptfs->Path);
    }

    free(Ptfs);
}

NTSTATUS VirtuelleFestplatte::SvcStart(FSP_SERVICE *Service, ULONG argc, PWSTR *argv) {
    PWSTR DebugLogFile = 0;
    ULONG DebugFlags = 0;
    PWSTR VolumePrefix = 0;
    PWSTR PassThrough = 0;
    PWSTR MountPoint = 0;

    HANDLE DebugLogHandle = INVALID_HANDLE_VALUE;
    WCHAR PassThroughBuf[MAX_PATH];
    PTFS *Ptfs = 0;
    NTSTATUS Result;

    PassThrough = const_cast<PWSTR>(orginalVolume.c_str());
    MountPoint = const_cast<PWSTR>(neuesVolume.c_str());

    EnableBackupRestorePrivileges();

    Result = PtfsCreate(PassThrough, VolumePrefix, MountPoint, DebugFlags, &Ptfs);
    if (isFail(Result, Ptfs)) {
        return Result;
    }

    Result = FspFileSystemStartDispatcher(Ptfs->FileSystem, 0);
    if (isFail(Result, Ptfs)) {
        return Result;
    }

    MountPoint = FspFileSystemMountPoint(Ptfs->FileSystem);

    Service->UserContext = Ptfs;
    Result = STATUS_SUCCESS;

    if (isFail(Result, Ptfs)) {
        return Result;
    }

    return Result;
}

NTSTATUS VirtuelleFestplatte::EnableBackupRestorePrivileges(VOID) {
    union {
        TOKEN_PRIVILEGES P;
        UINT8 B[sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES)];
    } Privileges;
    HANDLE Token;

    Privileges.P.PrivilegeCount = 2;
    Privileges.P.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    Privileges.P.Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

    if (!LookupPrivilegeValue(0, SE_BACKUP_NAME, &Privileges.P.Privileges[0].Luid) || // zu LookupPrivilegeValueW aendern bei error
        !LookupPrivilegeValue(0, SE_RESTORE_NAME, &Privileges.P.Privileges[1].Luid)) {
        return FspNtStatusFromWin32(GetLastError());
    }

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &Token)) {
        return FspNtStatusFromWin32(GetLastError());
    }

    if (!AdjustTokenPrivileges(Token, FALSE, &Privileges.P, 0, 0, 0)) {
        CloseHandle(Token);

        return FspNtStatusFromWin32(GetLastError());
    }

    CloseHandle(Token);

    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::PtfsCreate(PWSTR Path, PWSTR VolumePrefix, PWSTR MountPoint, UINT32 DebugFlags, PTFS **PPtfs) {

    WCHAR FullPath[MAX_PATH];
    ULONG Length;
    HANDLE Handle;
    FILETIME CreationTime;
    DWORD LastError;
    FSP_FSCTL_VOLUME_PARAMS VolumeParams;
    PTFS *Ptfs = 0;
    NTSTATUS Result;

    *PPtfs = 0;

    Handle = CreateFileW(
        Path, FILE_READ_ATTRIBUTES, 0, 0,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
    if (INVALID_HANDLE_VALUE == Handle) {
        return FspNtStatusFromWin32(GetLastError());
    }

    Length = GetFinalPathNameByHandleW(Handle, FullPath, FULLPATH_SIZE - 1, 0);
    if (0 == Length) {
        LastError = GetLastError();
        CloseHandle(Handle);
        return FspNtStatusFromWin32(LastError);
    }
    if (L'\\' == FullPath[Length - 1]) {
        FullPath[--Length] = L'\0';
    }

    if (!GetFileTime(Handle, &CreationTime, 0, 0)) {
        LastError = GetLastError();
        CloseHandle(Handle);
        return FspNtStatusFromWin32(LastError);
    }

    CloseHandle(Handle);

    /* from now on we must goto exit on failure */

    Ptfs = (PTFS *)malloc(sizeof *Ptfs);
    if (0 == Ptfs) {
        Result = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }
    memset(Ptfs, 0, sizeof *Ptfs);

    Length = (Length + 1) * sizeof(WCHAR);
    Ptfs->Path = (PWSTR)malloc(Length);
    if (0 == Ptfs->Path) {
        Result = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }
    memcpy(Ptfs->Path, FullPath, Length);

    memset(&VolumeParams, 0, sizeof VolumeParams);
    VolumeParams.SectorSize = ALLOCATION_UNIT;
    VolumeParams.SectorsPerAllocationUnit = 1;
    VolumeParams.VolumeCreationTime = ((PLARGE_INTEGER)&CreationTime)->QuadPart;
    VolumeParams.VolumeSerialNumber = 0;
    VolumeParams.FileInfoTimeout = 1000;
    VolumeParams.CaseSensitiveSearch = 0;
    VolumeParams.CasePreservedNames = 1;
    VolumeParams.UnicodeOnDisk = 1;
    VolumeParams.PersistentAcls = 1;
    VolumeParams.PostCleanupWhenModifiedOnly = 1;
    VolumeParams.PassQueryDirectoryPattern = 1;
    VolumeParams.FlushAndPurgeOnCleanup = 1;
    VolumeParams.UmFileContextIsUserContext2 = 1;
    if (0 != VolumePrefix) {
        wcscpy_s(VolumeParams.Prefix, sizeof VolumeParams.Prefix / sizeof(WCHAR), VolumePrefix);
        wcscpy_s(VolumeParams.FileSystemName, sizeof VolumeParams.FileSystemName / sizeof(WCHAR), L"" PROGNAME);
    }

    Result = FspFileSystemCreate(
        (PWSTR)(VolumeParams.Prefix[0] ? L"" FSP_FSCTL_NET_DEVICE_NAME : L"" FSP_FSCTL_DISK_DEVICE_NAME),
        &VolumeParams,
        &PtfsInterface,
        &Ptfs->FileSystem);
    if (!NT_SUCCESS(Result)) {
        goto exit;
    }
    Ptfs->FileSystem->UserContext = Ptfs;

    Result = FspFileSystemSetMountPoint(Ptfs->FileSystem, MountPoint);
    if (!NT_SUCCESS(Result)) {
        goto exit;
    }

    FspFileSystemSetDebugLog(Ptfs->FileSystem, DebugFlags);

    Result = STATUS_SUCCESS;

exit:
    if (NT_SUCCESS(Result)) {
        *PPtfs = Ptfs;
    } else if (0 != Ptfs) {
        PtfsDelete(Ptfs);
    }

    return Result;
}

// Gibt Zeiger auf statischen Puffer zurueck: z. B. L"F:\\"
LPCWSTR VirtuelleFestplatte::konvertExW(LPCWSTR path) {
    std::fill(std::begin(volumeRoot), std::end(volumeRoot), 0);
    // Pruefe: z. B. "\\?\F:\..." oder "F:\..."
    if (path[0] && path[1] == ':' && path[2] == '\\') {
        // Normaler Pfad: "F:\..."
        volumeRoot[0] = path[0]; // 'F'
        volumeRoot[1] = ':';
        volumeRoot[2] = '\\';
        volumeRoot[3] = '\0';
    } else if (wcsncmp(path, L"\\\\?\\", 4) == 0) {
        // Pfad im Format "\\?\F:\..."
        volumeRoot[0] = path[4]; // 'F'
        volumeRoot[1] = ':';
        volumeRoot[2] = '\\';
        volumeRoot[3] = '\0';
    }
    return volumeRoot;
}

NTSTATUS VirtuelleFestplatte::GetVolumeInfo(FSP_FILE_SYSTEM *FileSystem, FSP_FSCTL_VOLUME_INFO *VolumeInfo) {
    PTFS *Ptfs = (PTFS *)FileSystem->UserContext;
    WCHAR Root[MAX_PATH];
    ULARGE_INTEGER TotalSize, FreeSize;

    // windos funktin
    if (!GetVolumePathNameW(Ptfs->Path, Root, MAX_PATH)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    if (!GetDiskFreeSpaceExW(konvertExW(Ptfs->Path), 0, &TotalSize, &FreeSize)) {
        return FspNtStatusFromWin32(GetLastError());
    }

    VolumeInfo->TotalSize = TotalSize.QuadPart;
    VolumeInfo->FreeSize = FreeSize.QuadPart;

    /*
    printf("Passthrough-Pfad : %ls\n", Ptfs->Path);
    printf("VolumeInfo pointer: %p\n", (void*)VolumeInfo);
    printf("TotalSize        : %llu\n",
        (unsigned long long) VolumeInfo->TotalSize);
    printf("FreeSize         : %llu\n",
        (unsigned long long) VolumeInfo->FreeSize);
    */

    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::SetVolumeLabel_(FSP_FILE_SYSTEM *FileSystem,
                                              PWSTR VolumeLabel,
                                              FSP_FSCTL_VOLUME_INFO *VolumeInfo) {
    /* we do not support changing the volume label */
    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS VirtuelleFestplatte::GetSecurityByName(FSP_FILE_SYSTEM *FileSystem,
                                                PWSTR FileName, PUINT32 PFileAttributes,
                                                PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize) {
    PTFS *Ptfs = (PTFS *)FileSystem->UserContext;
    WCHAR FullPath[FULLPATH_SIZE];
    HANDLE Handle;
    FILE_ATTRIBUTE_TAG_INFO AttributeTagInfo;
    DWORD SecurityDescriptorSizeNeeded;
    NTSTATUS Result;

    if (!ConcatPath(Ptfs, FileName, FullPath)) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    Handle = CreateFileW(FullPath,
                         FILE_READ_ATTRIBUTES | READ_CONTROL, 0, 0,
                         OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
    if (INVALID_HANDLE_VALUE == Handle) {
        Result = FspNtStatusFromWin32(GetLastError());
        goto exit;
    }

    if (0 != PFileAttributes) {
        if (!GetFileInformationByHandleEx(Handle,
                                          FileAttributeTagInfo, &AttributeTagInfo, sizeof AttributeTagInfo)) {
            Result = FspNtStatusFromWin32(GetLastError());
            goto exit;
        }

        *PFileAttributes = AttributeTagInfo.FileAttributes;
    }

    if (0 != PSecurityDescriptorSize) {
        if (!GetKernelObjectSecurity(Handle,
                                     OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
                                     SecurityDescriptor, (DWORD)*PSecurityDescriptorSize, &SecurityDescriptorSizeNeeded)) {
            *PSecurityDescriptorSize = SecurityDescriptorSizeNeeded;
            Result = FspNtStatusFromWin32(GetLastError());
            goto exit;
        }

        *PSecurityDescriptorSize = SecurityDescriptorSizeNeeded;
    }

    Result = STATUS_SUCCESS;

exit:
    if (INVALID_HANDLE_VALUE != Handle) {
        CloseHandle(Handle);
    }
    return Result;
}

NTSTATUS VirtuelleFestplatte::Create(FSP_FILE_SYSTEM *FileSystem,
                                     PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess,
                                     UINT32 FileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, UINT64 AllocationSize,
                                     PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo) {
    PTFS *Ptfs = (PTFS *)FileSystem->UserContext;
    WCHAR FullPath[FULLPATH_SIZE];
    SECURITY_ATTRIBUTES SecurityAttributes;
    ULONG CreateFlags;
    PTFS_FILE_CONTEXT *FileContext;

    if (!ConcatPath(Ptfs, FileName, FullPath)) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    FileContext = (PTFS_FILE_CONTEXT *)malloc(sizeof *FileContext);
    if (0 == FileContext) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    memset(FileContext, 0, sizeof *FileContext);

    SecurityAttributes.nLength = sizeof SecurityAttributes;
    SecurityAttributes.lpSecurityDescriptor = SecurityDescriptor;
    SecurityAttributes.bInheritHandle = FALSE;

    CreateFlags = FILE_FLAG_BACKUP_SEMANTICS;
    if (CreateOptions & FILE_DELETE_ON_CLOSE) {
        CreateFlags |= FILE_FLAG_DELETE_ON_CLOSE;
    }
    if (CreateOptions & FILE_DIRECTORY_FILE) {
        /*
         * It is not widely known but CreateFileW can be used to create directories!
         * It requires the specification of both FILE_FLAG_BACKUP_SEMANTICS and
         * FILE_FLAG_POSIX_SEMANTICS. It also requires that FileAttributes has
         * FILE_ATTRIBUTE_DIRECTORY set.
         */
        CreateFlags |= FILE_FLAG_POSIX_SEMANTICS;
        FileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
    } else {
        FileAttributes &= ~FILE_ATTRIBUTE_DIRECTORY;
    }

    if (0 == FileAttributes) {
        FileAttributes = FILE_ATTRIBUTE_NORMAL;
    }

    FileContext->Handle = CreateFileW(FullPath,
                                      GrantedAccess, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, &SecurityAttributes,
                                      CREATE_NEW, CreateFlags | FileAttributes, 0);
    if (INVALID_HANDLE_VALUE == FileContext->Handle) {
        free(FileContext);
        return FspNtStatusFromWin32(GetLastError());
    }

    *PFileContext = FileContext;

    return GetFileInfoInternal(FileContext->Handle, FileInfo);
}

NTSTATUS VirtuelleFestplatte::Open(FSP_FILE_SYSTEM *FileSystem,
                                   PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess,
                                   PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo) {
    PTFS *Ptfs = (PTFS *)FileSystem->UserContext;
    WCHAR FullPath[FULLPATH_SIZE];
    ULONG CreateFlags;
    PTFS_FILE_CONTEXT *FileContext;

    if (!ConcatPath(Ptfs, FileName, FullPath)) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    FileContext = (PTFS_FILE_CONTEXT *)malloc(sizeof *FileContext);
    if (0 == FileContext) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    memset(FileContext, 0, sizeof *FileContext);

    CreateFlags = FILE_FLAG_BACKUP_SEMANTICS;
    if (CreateOptions & FILE_DELETE_ON_CLOSE) {
        CreateFlags |= FILE_FLAG_DELETE_ON_CLOSE;
    }

    FileContext->Handle = CreateFileW(FullPath,
                                      GrantedAccess, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0,
                                      OPEN_EXISTING, CreateFlags, 0);
    if (INVALID_HANDLE_VALUE == FileContext->Handle) {
        free(FileContext);
        return FspNtStatusFromWin32(GetLastError());
    }

    *PFileContext = FileContext;

    // std::wcout << L"Open Pfad : " << FileName << std::endl;

    return GetFileInfoInternal(FileContext->Handle, FileInfo);
}

NTSTATUS VirtuelleFestplatte::Overwrite(FSP_FILE_SYSTEM *FileSystem,
                                        PVOID FileContext, UINT32 FileAttributes, BOOLEAN ReplaceFileAttributes, UINT64 AllocationSize,
                                        FSP_FSCTL_FILE_INFO *FileInfo) {
    HANDLE Handle = HandleFromContext(FileContext);
    FILE_BASIC_INFO BasicInfo = {0};
    FILE_ALLOCATION_INFO AllocationInfo = {0};
    FILE_ATTRIBUTE_TAG_INFO AttributeTagInfo;

    if (ReplaceFileAttributes) {
        if (0 == FileAttributes) {
            FileAttributes = FILE_ATTRIBUTE_NORMAL;
        }

        BasicInfo.FileAttributes = FileAttributes;
        if (!SetFileInformationByHandle(Handle, FileBasicInfo, &BasicInfo, sizeof BasicInfo)) {
            return FspNtStatusFromWin32(GetLastError());
        }
    } else if (0 != FileAttributes) {
        if (!GetFileInformationByHandleEx(Handle, FileAttributeTagInfo, &AttributeTagInfo, sizeof AttributeTagInfo)) {
            return FspNtStatusFromWin32(GetLastError());
        }

        BasicInfo.FileAttributes = FileAttributes | AttributeTagInfo.FileAttributes;
        if (BasicInfo.FileAttributes ^ FileAttributes) {
            if (!SetFileInformationByHandle(Handle, FileBasicInfo, &BasicInfo, sizeof BasicInfo)) {
                return FspNtStatusFromWin32(GetLastError());
            }
        }
    }

    if (!SetFileInformationByHandle(Handle, FileAllocationInfo, &AllocationInfo, sizeof AllocationInfo)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    cache.RemoveHandle(Handle);
    return GetFileInfoInternal(Handle, FileInfo);
}

VOID VirtuelleFestplatte::Cleanup(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, ULONG Flags) {
    HANDLE Handle = HandleFromContext(FileContext);

    if (Flags & FspCleanupDelete) {
        CloseHandle(Handle);

        /* this will make all future uses of Handle to fail with STATUS_INVALID_HANDLE */
        HandleFromContext(FileContext) = INVALID_HANDLE_VALUE;
    }
}

VOID VirtuelleFestplatte::Close(FSP_FILE_SYSTEM *FileSystem,
                                PVOID FileContext0) {
    PTFS_FILE_CONTEXT *FileContext = (PTFS_FILE_CONTEXT *)FileContext0;
    HANDLE Handle = HandleFromContext(FileContext);

    CloseHandle(Handle);

    FspFileSystemDeleteDirectoryBuffer(&FileContext->DirBuffer);
    free(FileContext);
}

NTSTATUS VirtuelleFestplatte::Read(FSP_FILE_SYSTEM *FileSystem,
                                   PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length,
                                   PULONG PBytesTransferred) {
    HANDLE Handle = HandleFromContext(FileContext);
    OVERLAPPED Overlapped = {0};

    Overlapped.Offset = (DWORD)Offset;
    Overlapped.OffsetHigh = (DWORD)(Offset >> 32);

    if (!cache.Read(Handle, Buffer, Length, PBytesTransferred, &Overlapped)) {
        if (!ReadFile(Handle, Buffer, Length, PBytesTransferred, &Overlapped)) {
            return FspNtStatusFromWin32(GetLastError());
        }
    } else {
        // std::cout << "Read : von cash " << std::endl;
    }
    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::Write(FSP_FILE_SYSTEM *FileSystem,
                                    PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length,
                                    BOOLEAN WriteToEndOfFile, BOOLEAN ConstrainedIo,
                                    PULONG PBytesTransferred, FSP_FSCTL_FILE_INFO *FileInfo) {
    HANDLE Handle = HandleFromContext(FileContext);
    LARGE_INTEGER FileSize;
    OVERLAPPED Overlapped = {0};

    if (ConstrainedIo) {
        if (!GetFileSizeEx(Handle, &FileSize)) {
            return FspNtStatusFromWin32(GetLastError());
        }

        if (Offset >= (UINT64)FileSize.QuadPart) {
            return STATUS_SUCCESS;
        }
        if (Offset + Length > (UINT64)FileSize.QuadPart) {
            Length = (ULONG)((UINT64)FileSize.QuadPart - Offset);
        }
    }

    Overlapped.Offset = (DWORD)Offset;
    Overlapped.OffsetHigh = (DWORD)(Offset >> 32);

    if (!WriteFile(Handle, Buffer, Length, PBytesTransferred, &Overlapped)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    cache.RemoveHandle(Handle);
    return GetFileInfoInternal(Handle, FileInfo);
}

NTSTATUS VirtuelleFestplatte::Flush(FSP_FILE_SYSTEM *FileSystem,
                                    PVOID FileContext,
                                    FSP_FSCTL_FILE_INFO *FileInfo) {
    HANDLE Handle = HandleFromContext(FileContext);

    /* we do not flush the whole volume, so just return SUCCESS */
    if (0 == Handle) {
        return STATUS_SUCCESS;
    }
    if (!FlushFileBuffers(Handle)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    return GetFileInfoInternal(Handle, FileInfo);
}

NTSTATUS VirtuelleFestplatte::GetFileInfo(FSP_FILE_SYSTEM *FileSystem,
                                          PVOID FileContext,
                                          FSP_FSCTL_FILE_INFO *FileInfo) {
    HANDLE Handle = HandleFromContext(FileContext);

    return GetFileInfoInternal(Handle, FileInfo);
}

NTSTATUS VirtuelleFestplatte::SetBasicInfo(FSP_FILE_SYSTEM *FileSystem,
                                           PVOID FileContext, UINT32 FileAttributes,
                                           UINT64 CreationTime, UINT64 LastAccessTime, UINT64 LastWriteTime, UINT64 ChangeTime,
                                           FSP_FSCTL_FILE_INFO *FileInfo) {
    HANDLE Handle = HandleFromContext(FileContext);
    FILE_BASIC_INFO BasicInfo = {0};

    if (INVALID_FILE_ATTRIBUTES == FileAttributes)
        FileAttributes = 0;
    else if (0 == FileAttributes)
        FileAttributes = FILE_ATTRIBUTE_NORMAL;

    BasicInfo.FileAttributes = FileAttributes;
    BasicInfo.CreationTime.QuadPart = CreationTime;
    BasicInfo.LastAccessTime.QuadPart = LastAccessTime;
    BasicInfo.LastWriteTime.QuadPart = LastWriteTime;
    // BasicInfo.ChangeTime = ChangeTime;

    if (!SetFileInformationByHandle(Handle,
                                    FileBasicInfo, &BasicInfo, sizeof BasicInfo))
        return FspNtStatusFromWin32(GetLastError());

    return GetFileInfoInternal(Handle, FileInfo);
}

NTSTATUS VirtuelleFestplatte::SetFileSize(FSP_FILE_SYSTEM *FileSystem,
                                          PVOID FileContext, UINT64 NewSize, BOOLEAN SetAllocationSize,
                                          FSP_FSCTL_FILE_INFO *FileInfo) {
    HANDLE Handle = HandleFromContext(FileContext);
    FILE_ALLOCATION_INFO AllocationInfo;
    FILE_END_OF_FILE_INFO EndOfFileInfo;

    if (SetAllocationSize) {
        /*
         * This file system does not maintain AllocationSize, although NTFS clearly can.
         * However it must always be FileSize <= AllocationSize and NTFS will make sure
         * to truncate the FileSize if it sees an AllocationSize < FileSize.
         *
         * If OTOH a very large AllocationSize is passed, the call below will increase
         * the AllocationSize of the underlying file, although our file system does not
         * expose this fact. This AllocationSize is only temporary as NTFS will reset
         * the AllocationSize of the underlying file when it is closed.
         */

        AllocationInfo.AllocationSize.QuadPart = NewSize;

        if (!SetFileInformationByHandle(Handle,
                                        FileAllocationInfo, &AllocationInfo, sizeof AllocationInfo))
            return FspNtStatusFromWin32(GetLastError());
    } else {
        EndOfFileInfo.EndOfFile.QuadPart = NewSize;

        if (!SetFileInformationByHandle(Handle,
                                        FileEndOfFileInfo, &EndOfFileInfo, sizeof EndOfFileInfo))
            return FspNtStatusFromWin32(GetLastError());
    }
    cache.RemoveHandle(Handle);
    return GetFileInfoInternal(Handle, FileInfo);
}

NTSTATUS VirtuelleFestplatte::Rename(FSP_FILE_SYSTEM *FileSystem,
                                     PVOID FileContext,
                                     PWSTR FileName, PWSTR NewFileName, BOOLEAN ReplaceIfExists) {
    PTFS *Ptfs = (PTFS *)FileSystem->UserContext;
    WCHAR FullPath[FULLPATH_SIZE], NewFullPath[FULLPATH_SIZE];

    if (!ConcatPath(Ptfs, FileName, FullPath))
        return STATUS_OBJECT_NAME_INVALID;

    if (!ConcatPath(Ptfs, NewFileName, NewFullPath))
        return STATUS_OBJECT_NAME_INVALID;

    if (!MoveFileExW(FullPath, NewFullPath, ReplaceIfExists ? MOVEFILE_REPLACE_EXISTING : 0)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    cache.Remove(FullPath);
    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::GetSecurity(FSP_FILE_SYSTEM *FileSystem,
                                          PVOID FileContext,
                                          PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize) {
    HANDLE Handle = HandleFromContext(FileContext);
    DWORD SecurityDescriptorSizeNeeded;

    if (!GetKernelObjectSecurity(Handle,
                                 OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
                                 SecurityDescriptor, (DWORD)*PSecurityDescriptorSize, &SecurityDescriptorSizeNeeded)) {
        *PSecurityDescriptorSize = SecurityDescriptorSizeNeeded;
        return FspNtStatusFromWin32(GetLastError());
    }

    *PSecurityDescriptorSize = SecurityDescriptorSizeNeeded;

    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::SetSecurity(FSP_FILE_SYSTEM *FileSystem,
                                          PVOID FileContext,
                                          SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR ModificationDescriptor) {
    HANDLE Handle = HandleFromContext(FileContext);

    if (!SetKernelObjectSecurity(Handle, SecurityInformation, ModificationDescriptor)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    cache.RemoveHandle(Handle);
    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::ReadDirectory(FSP_FILE_SYSTEM *FileSystem,
                                            PVOID FileContext0, PWSTR Pattern, PWSTR Marker,
                                            PVOID Buffer, ULONG BufferLength, PULONG PBytesTransferred) {
    PTFS *Ptfs = (PTFS *)FileSystem->UserContext;
    PTFS_FILE_CONTEXT *FileContext = (PTFS_FILE_CONTEXT *)FileContext0;
    HANDLE Handle = HandleFromContext(FileContext);
    WCHAR FullPath[FULLPATH_SIZE];
    ULONG Length, PatternLength;
    HANDLE FindHandle;
    WIN32_FIND_DATAW FindData;
    union {
        UINT8 B[FIELD_OFFSET(FSP_FSCTL_DIR_INFO, FileNameBuf) + MAX_PATH * sizeof(WCHAR)];
        FSP_FSCTL_DIR_INFO D;
    } DirInfoBuf;
    FSP_FSCTL_DIR_INFO *DirInfo = &DirInfoBuf.D;
    NTSTATUS DirBufferResult;

    DirBufferResult = STATUS_SUCCESS;
    if (FspFileSystemAcquireDirectoryBuffer(&FileContext->DirBuffer, 0 == Marker, &DirBufferResult)) {
        if (0 == Pattern) {
            Pattern = (PWSTR)L"*";
        }
        PatternLength = (ULONG)wcslen(Pattern);

        Length = GetFinalPathNameByHandleW(Handle, FullPath, FULLPATH_SIZE - 1, 0);
        if (0 == Length) {
            DirBufferResult = FspNtStatusFromWin32(GetLastError());
        } else if (Length + 1 + PatternLength >= FULLPATH_SIZE) {
            DirBufferResult = STATUS_OBJECT_NAME_INVALID;
        }
        if (!NT_SUCCESS(DirBufferResult)) {
            FspFileSystemReleaseDirectoryBuffer(&FileContext->DirBuffer);
            return DirBufferResult;
        }

        if (L'\\' != FullPath[Length - 1]) {
            FullPath[Length++] = L'\\';
        }
        memcpy(FullPath + Length, Pattern, PatternLength * sizeof(WCHAR));
        FullPath[Length + PatternLength] = L'\0';

        FindHandle = FindFirstFileW(FullPath, &FindData);
        if (INVALID_HANDLE_VALUE != FindHandle) {
            do {
                memset(DirInfo, 0, sizeof *DirInfo);
                Length = (ULONG)wcslen(FindData.cFileName);
                DirInfo->Size = (UINT16)(FIELD_OFFSET(FSP_FSCTL_DIR_INFO, FileNameBuf) + Length * sizeof(WCHAR));
                DirInfo->FileInfo.FileAttributes = FindData.dwFileAttributes;
                DirInfo->FileInfo.ReparseTag = 0;
                DirInfo->FileInfo.FileSize =
                    ((UINT64)FindData.nFileSizeHigh << 32) | (UINT64)FindData.nFileSizeLow;
                DirInfo->FileInfo.AllocationSize = (DirInfo->FileInfo.FileSize + ALLOCATION_UNIT - 1) / ALLOCATION_UNIT * ALLOCATION_UNIT;
                DirInfo->FileInfo.CreationTime = ((PLARGE_INTEGER)&FindData.ftCreationTime)->QuadPart;
                DirInfo->FileInfo.LastAccessTime = ((PLARGE_INTEGER)&FindData.ftLastAccessTime)->QuadPart;
                DirInfo->FileInfo.LastWriteTime = ((PLARGE_INTEGER)&FindData.ftLastWriteTime)->QuadPart;
                DirInfo->FileInfo.ChangeTime = DirInfo->FileInfo.LastWriteTime;
                DirInfo->FileInfo.IndexNumber = 0;
                DirInfo->FileInfo.HardLinks = 0;
                memcpy(DirInfo->FileNameBuf, FindData.cFileName, Length * sizeof(WCHAR));

                if (!FspFileSystemFillDirectoryBuffer(&FileContext->DirBuffer, DirInfo, &DirBufferResult)) {
                    break;
                }
            } while (FindNextFileW(FindHandle, &FindData));

            FindClose(FindHandle);
        }

        FspFileSystemReleaseDirectoryBuffer(&FileContext->DirBuffer);
    }

    if (!NT_SUCCESS(DirBufferResult)) {
        return DirBufferResult;
    }

    FspFileSystemReadDirectoryBuffer(&FileContext->DirBuffer, Marker, Buffer, BufferLength, PBytesTransferred);

    // std::wcout << "ReadDirectory : " << FullPath << std::endl;

    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::SetDelete(FSP_FILE_SYSTEM *FileSystem,
                                        PVOID FileContext, PWSTR FileName, BOOLEAN DeleteFile) {
    HANDLE Handle = HandleFromContext(FileContext);
    FILE_DISPOSITION_INFO DispositionInfo;

    DispositionInfo.DeleteFile = DeleteFile;

    if (!SetFileInformationByHandle(Handle,
                                    FileDispositionInfo, &DispositionInfo, sizeof DispositionInfo)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    cache.RemoveHandle(Handle);
    return STATUS_SUCCESS;
}

NTSTATUS VirtuelleFestplatte::GetFileInfoInternal(HANDLE Handle, FSP_FSCTL_FILE_INFO *FileInfo) {
    BY_HANDLE_FILE_INFORMATION ByHandleFileInfo;
    if (!GetFileInformationByHandle(Handle, &ByHandleFileInfo)) {
        return FspNtStatusFromWin32(GetLastError());
    }
    FileInfo->FileAttributes = ByHandleFileInfo.dwFileAttributes;
    FileInfo->ReparseTag = 0;
    FileInfo->FileSize = ((UINT64)ByHandleFileInfo.nFileSizeHigh << 32) | (UINT64)ByHandleFileInfo.nFileSizeLow;
    FileInfo->AllocationSize = (FileInfo->FileSize + ALLOCATION_UNIT - 1) / ALLOCATION_UNIT * ALLOCATION_UNIT;
    FileInfo->CreationTime = ((PLARGE_INTEGER)&ByHandleFileInfo.ftCreationTime)->QuadPart;
    FileInfo->LastAccessTime = ((PLARGE_INTEGER)&ByHandleFileInfo.ftLastAccessTime)->QuadPart;
    FileInfo->LastWriteTime = ((PLARGE_INTEGER)&ByHandleFileInfo.ftLastWriteTime)->QuadPart;
    FileInfo->ChangeTime = FileInfo->LastWriteTime;
    FileInfo->IndexNumber = ((UINT64)ByHandleFileInfo.nFileIndexHigh << 32) | (UINT64)ByHandleFileInfo.nFileIndexLow;
    FileInfo->HardLinks = 0;

    return STATUS_SUCCESS;
}
