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
    CacheInterface &cache;
    FSP_FILE_SYSTEM_INTERFACE PtfsInterface;
    WCHAR volumeRoot[4];

public:
    /**
     * @brief ersellt ein neues Vituelles Volume
     *
     * @param orginalVolume das Volume wo die daten ligen.
     * @param neuesVolume das neue Volume wo die Daten von orginalVolume aber mit Cache sind.
     * @param cache ist die Implementierung der Cache-Logik.
     */
    VirtuelleFestplatte(std::wstring orginalVolume, std::wstring neuesVolume, Cache &cache);
    void start();
    void stop();

private:
    LPCWSTR konvertExW(LPCWSTR path);
    bool isFail(NTSTATUS result, PTFS *ptfs);
    VOID PtfsDelete(PTFS *Ptfs);
    NTSTATUS EnableBackupRestorePrivileges(VOID);
    NTSTATUS PtfsCreate(PWSTR Path, PWSTR VolumePrefix, PWSTR MountPoint, UINT32 DebugFlags, PTFS **PPtfs);

public:
    // PtfsInterface
    NTSTATUS SvcStart(FSP_SERVICE *Service, ULONG argc, PWSTR *argv);
    NTSTATUS SvcStop(FSP_SERVICE *Service);
    NTSTATUS GetVolumeInfo(FSP_FILE_SYSTEM *FileSystem, FSP_FSCTL_VOLUME_INFO *VolumeInfo);
    NTSTATUS SetVolumeLabel_(FSP_FILE_SYSTEM *FileSystem, PWSTR VolumeLabel, FSP_FSCTL_VOLUME_INFO *VolumeInfo);
    NTSTATUS GetSecurityByName(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, PUINT32 PFileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize);
    NTSTATUS Create(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess, UINT32 FileAttributes, PSECURITY_DESCRIPTOR SecurityDescriptor, UINT64 AllocationSize, PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS Open(FSP_FILE_SYSTEM *FileSystem, PWSTR FileName, UINT32 CreateOptions, UINT32 GrantedAccess, PVOID *PFileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS Overwrite(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT32 FileAttributes, BOOLEAN ReplaceFileAttributes, UINT64 AllocationSize, FSP_FSCTL_FILE_INFO *FileInfo);
    VOID Cleanup(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, ULONG Flags);
    VOID Close(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext0);
    NTSTATUS Read(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length, PULONG PBytesTransferred);
    NTSTATUS Write(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PVOID Buffer, UINT64 Offset, ULONG Length, BOOLEAN WriteToEndOfFile, BOOLEAN ConstrainedIo, PULONG PBytesTransferred, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS Flush(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS GetFileInfo(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS SetBasicInfo(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT32 FileAttributes, UINT64 CreationTime, UINT64 LastAccessTime, UINT64 LastWriteTime, UINT64 ChangeTime, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS SetFileSize(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, UINT64 NewSize, BOOLEAN SetAllocationSize, FSP_FSCTL_FILE_INFO *FileInfo);
    NTSTATUS Rename(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, PWSTR NewFileName, BOOLEAN ReplaceIfExists);
    NTSTATUS GetSecurity(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PSECURITY_DESCRIPTOR SecurityDescriptor, SIZE_T *PSecurityDescriptorSize);
    NTSTATUS SetSecurity(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR ModificationDescriptor);
    NTSTATUS ReadDirectory(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext0, PWSTR Pattern, PWSTR Marker, PVOID Buffer, ULONG BufferLength, PULONG PBytesTransferred);
    NTSTATUS SetDelete(FSP_FILE_SYSTEM *FileSystem, PVOID FileContext, PWSTR FileName, BOOLEAN DeleteFile);
    NTSTATUS GetFileInfoInternal(HANDLE Handle, FSP_FSCTL_FILE_INFO *FileInfo);
};
