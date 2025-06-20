// VirtuelleFestplatte.h
#pragma once
#include "Cache.h"
#include <string>
#include <strsafe.h>
#include <winfsp/winfsp.h>

struct VirtuelleFestplatte;
typedef struct {
    FSP_FILE_SYSTEM *FileSystem;
    PWSTR Path;
    VirtuelleFestplatte *vhdd;
} PTFS;

class VirtuelleFestplatte {
private:
    std::wstring orginalVolume;
    std::wstring neuesVolume;
    CacheInterface &cache;
    FSP_FILE_SYSTEM_INTERFACE PtfsInterface;
    WCHAR volumeRoot[4];

public:
    FSP_SERVICE *fspService = nullptr;

public:
    /**
     * @brief ersellt ein neues Vituelles Volume
     *
     * @param orginalVolume das Volume wo die daten ligen.
     * @param neuesVolume das neue Volume wo die Daten von orginalVolume aber mit Cache sind.
     * @param cache ist die Implementierung der Cache-Logik.
     */
    VirtuelleFestplatte(std::wstring orginalVolume, std::wstring neuesVolume, CacheInterface &cache);
    void start();
    std::wstring getOrginalVolume() { return orginalVolume; };
    std::wstring getNeuesVolume() { return neuesVolume; };
    CacheInterface &getCache() { return cache; };

private:
    LPCWSTR konvertExW(LPCWSTR path);
    bool isFail(NTSTATUS result, PTFS *ptfs);
    void erstelleVhdd();
    VOID ptfsDelete(PTFS *Ptfs);
    NTSTATUS enableBackupRestorePrivileges(VOID);
    NTSTATUS ptfsCreate(PWSTR Path, PWSTR VolumePrefix, PWSTR MountPoint, UINT32 DebugFlags, PTFS **PPtfs);

public:
    // PtfsInterface
    NTSTATUS svcStart(FSP_SERVICE *Service, ULONG argc, PWSTR *argv);
    NTSTATUS svcStop(FSP_SERVICE *Service);
    NTSTATUS getVolumeInfo(FSP_FILE_SYSTEM *FileSystem, FSP_FSCTL_VOLUME_INFO *VolumeInfo);
    NTSTATUS setVolumeLabel_(FSP_FILE_SYSTEM *FileSystem, PWSTR VolumeLabel, FSP_FSCTL_VOLUME_INFO *VolumeInfo);
    NTSTATUS getSecurityByName(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, PUINT32 PFileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize);
    NTSTATUS create(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess, UINT32 FileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, UINT64 AllocationSize, PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS open(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess, PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS overwrite(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT32 FileAttributes, BOOLEAN ReplaceFileAttributes, UINT64 AllocationSize, FSP_FSCTL_FILE_INFO *FileInfo);
    VOID cleanup(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, ULONG Flags);
    VOID close(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext0);
    NTSTATUS read(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length, PULONG PBytesTransferred);
    NTSTATUS write(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length, BOOLEAN WriteToEndOfFile, BOOLEAN ConstrainedIo, PULONG PBytesTransferred, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS flush(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS getFileInfo(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS setBasicInfo(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT32 FileAttributes, UINT64 CreationTime, UINT64 LastAccessTime, UINT64 LastWriteTime, UINT64 ChangeTime, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS setFileSize(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT64 NewSize, BOOLEAN SetAllocationSize, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS rename(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, PWSTR NewFileName, BOOLEAN ReplaceIfExists);
    NTSTATUS getSecurity(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize);
    NTSTATUS setSecurity(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR ModificationDescriptor);
    NTSTATUS readDirectory(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext0, PWSTR Pattern, PWSTR Marker, PVOID Buffer, ULONG BufferLength, PULONG PBytesTransferred);
    NTSTATUS setDelete(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, BOOLEAN DeleteFile);
    NTSTATUS getFileInfoInternal(HANDLE Handle, FSP_FSCTL_FILE_INFO *FileInfo);
};
