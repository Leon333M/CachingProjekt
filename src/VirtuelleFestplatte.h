// VirtuelleFestplatte.h
#pragma once
#include "Cache.h"
#include <string>
#include <strsafe.h>
#include <winfsp/winfsp.h>

typedef struct {
    FSP_FILE_SYSTEM *FileSystem;
    PWSTR Path;
} PTFS;
typedef struct {
    HANDLE Handle;
    PVOID DirBuffer;
} PTFS_FILE_CONTEXT;

class VirtuelleFestplatte {
private:
    std::wstring orginalVolume;
    std::wstring neuesVolume;
    std::wstring cacheVolume;

public:
    /**
     * @brief ersellt ein neues Vituelles Volume
     *
     * @param orginalVolume das Volume wo die daten ligen.
     * @param neuesVolume das neue Volume wo die Daten von orginalVolume aber mit Cache sind.
     * @param cacheVolume das Volume was zum cashen genommen wird
     */
    VirtuelleFestplatte(std::wstring orginalVolume, std::wstring neuesVolume, std::wstring cacheVolume);
    void start();
    void stop();

private:
    static NTSTATUS SvcStart(FSP_SERVICE *Service, ULONG argc, PWSTR *argv);
    static NTSTATUS SvcStop(FSP_SERVICE *Service);
    static VOID PtfsDelete(PTFS *Ptfs);
    static NTSTATUS EnableBackupRestorePrivileges(VOID);
    static NTSTATUS PtfsCreate(PWSTR Path, PWSTR VolumePrefix, PWSTR MountPoint, UINT32 DebugFlags, PTFS **PPtfs);
};
/*
    // PtfsInterface
    static NTSTATUS GetVolumeInfo(FSP_FILE_SYSTEM *FileSystem, FSP_FSCTL_VOLUME_INFO *VolumeInfo);
    static NTSTATUS SetVolumeLabel_(FSP_FILE_SYSTEM *FileSystem, PWSTR VolumeLabel, FSP_FSCTL_VOLUME_INFO *VolumeInfo);
    static NTSTATUS GetSecurityByName(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, PUINT32 PFileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize);
    static NTSTATUS Create(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess, UINT32 FileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, UINT64 AllocationSize, PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    static NTSTATUS Open(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess, PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    static NTSTATUS Overwrite(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT32 FileAttributes, BOOLEAN ReplaceFileAttributes, UINT64 AllocationSize, FSP_FSCTL_FILE_INFO *FileInfo);
    static VOID Cleanup(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, ULONG Flags);
    static VOID Close(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext0);
    static NTSTATUS Read(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length, PULONG PBytesTransferred);
    static NTSTATUS Write(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length,BOOLEAN WriteToEndOfFile, BOOLEAN ConstrainedIo,PULONG PBytesTransferred, FSP_FSCTL_FILE_INFO *FileInfo);
    static NTSTATUS Flush(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext,FSP_FSCTL_FILE_INFO *FileInfo);
    static NTSTATUS GetFileInfo(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext,FSP_FSCTL_FILE_INFO *FileInfo);
    static NTSTATUS SetBasicInfo(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext, UINT32 FileAttributes,UINT64 CreationTime, UINT64 LastAccessTime, UINT64 LastWriteTime, UINT64 ChangeTime,FSP_FSCTL_FILE_INFO *FileInfo);
    static NTSTATUS SetFileSize(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext, UINT64 NewSize, BOOLEAN SetAllocationSize,FSP_FSCTL_FILE_INFO *FileInfo);
    static NTSTATUS Rename(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext,PWSTR FileName, PWSTR NewFileName, BOOLEAN ReplaceIfExists);
    static NTSTATUS GetSecurity(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext,PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize);
    static NTSTATUS SetSecurity(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext,SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR ModificationDescriptor);
    static NTSTATUS ReadDirectory(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext0, PWSTR Pattern, PWSTR Marker,PVOID Buffer, ULONG BufferLength, PULONG PBytesTransferred);
    static NTSTATUS SetDelete(FSP_FILE_SYSTEM *FileSystem,PVOID FileContext, PWSTR FileName, BOOLEAN DeleteFile);
    static NTSTATUS GetFileInfoInternal(HANDLE Handle, FSP_FSCTL_FILE_INFO *FileInfo);
    static FSP_FILE_SYSTEM_INTERFACE PtfsInterface = {
        .GetVolumeInfo = GetVolumeInfo,
        .SetVolumeLabel = SetVolumeLabel_,
        .GetSecurityByName = GetSecurityByName,
        .Create = Create,
        .Open = Open,
        .Overwrite = Overwrite,
        .Cleanup = Cleanup,
        .Close = Close,
        .Read = Read,
        .Write = Write,
        .Flush = Flush,
        .GetFileInfo = GetFileInfo,
        .SetBasicInfo = SetBasicInfo,
        .SetFileSize = SetFileSize,
        .Rename = Rename,
        .GetSecurity = GetSecurity,
        .SetSecurity = SetSecurity,
        .ReadDirectory = ReadDirectory,
        .SetDelete = SetDelete,
};

*/
