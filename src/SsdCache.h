// SsdCache.h
#pragma once
#include "CacheInterface.h"

class SsdCache : public CacheInterface {
private:
    std::wstring cacheVolume = L"E:";
    std::wstring cacheStammVerzeichnis = cacheVolume + L"/Cashe/";

public:
    SsdCache(std::wstring ssdCacheValue, UINT64 maxCacheSizeInGb, int minZugriffsHaufigkeit = 10);
    void Remove(const std::wstring &fullPath);

private:
    bool readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool storeInCache(const std::wstring &fullPath, HANDLE handle);

    // eigene Funktionen
    bool readSsdCache(const std::wstring &cachePath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool storeInSsdCache(const std::wstring &originalPath, std::wstring &cachePath);
    void clearCacheVerzeichnis();
    std::wstring GetCachePathFromFullPath(const std::wstring &fullPath);
};
