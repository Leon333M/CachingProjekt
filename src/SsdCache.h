// SsdCache.h
#pragma once
#include "CacheInterface.h"

class SsdCache : public CacheInterface {
private:
    std::wstring cacheVolume = L"E:";
    std::wstring cacheStammVerzeichnis = cacheVolume + L"/Cashe/" + cacheName + L"/";

public:
    SsdCache(std::wstring name, std::wstring ssdCacheValue, UINT64 maxCacheSizeInGb, int minZugriffsHaufigkeit = 10, int maxPfadHistorie = 64);
    std::wstring getCacheVolume();
    std::wstring getCacheStammVerzeichnis();

private:
    bool readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool storeInCache(const std::wstring &fullPath, HANDLE handle);
    bool removeCache(const std::wstring &fullPath);

    // eigene Funktionen
    bool readSsdCache(const std::wstring &cachePath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool storeInSsdCache(const std::wstring &originalPath, std::wstring &cachePath);
    void clearCacheVerzeichnis();
    std::wstring getCachePathFromFullPath(const std::wstring &fullPath);
};
