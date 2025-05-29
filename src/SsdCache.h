// SsdCache.h
#pragma once
#include "CacheInterface.h"
#include <deque>
#include <unordered_set>

class SsdCache : public CacheInterface {
private:
    std::unordered_set<std::wstring> cashePfade;
    std::deque<std::wstring> pfadHistorie;
    const int minZugriffsHaufigkeit = 2;
    const int maxPfadHistorie = 100;
    std::wstring cacheVolume = L"E:";
    std::wstring cacheStammVerzeichnis = cacheVolume + L"/Cashe/";

public:
    SsdCache(std::wstring ssdCacheValue, UINT64 maxCacheSizeInGb);
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring &fullPath);
    void Clear();

private:
    std::wstring GetCachePathFromFullPath(const std::wstring &fullPath);
    bool AddFile(std::wstring &originalPath, std::wstring &cachePath, HANDLE handle, WCHAR fullPath[1284]);
    bool AddFileSize(const UINT64 &fileSize);
    UINT64 Clear(const size_t &size);
    UINT64 SizeFromPath(const std::wstring &Path);
    bool ShouldCachePath(const std::wstring &fullPath);
};
