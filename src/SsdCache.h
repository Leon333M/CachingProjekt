// SsdCache.h
#pragma once
#include "CacheInterface.h"

class SsdCache : public CacheInterface {
private:
    std::wstring cacheVolume = L"E:";
    std::wstring cacheStammVerzeichnis = cacheVolume + L"/Cashe/";

public:
    SsdCache(std::wstring ssdCacheValue, UINT64 maxCacheSizeInGb, int minZugriffsHaufigkeit = 10);
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring &fullPath);

private:
    std::wstring GetCachePathFromFullPath(const std::wstring &fullPath);
    bool AddFile(std::wstring &originalPath, std::wstring &cachePath, HANDLE handle, WCHAR fullPath[1284]);
    bool AddFileSize(const UINT64 &fileSize);
    void clearCacheVerzeichnis();
};
