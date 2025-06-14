// Cache.h
#pragma once
#include "RamCache.h"
#include "SsdCache.h"

class Cache : public CacheInterface {
private:
    CacheInterface &ssdCache;
    CacheInterface &ramCache;

public:
    Cache(std::wstring name, CacheInterface &ramCache, CacheInterface &ssdCache);
    bool read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void remove(const std::wstring &fullPath);
    bool isCached(const std::wstring &fullPath) const;

private:
    // eigene Funktionen
    // ungenutze Funktionen
    bool readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) { return false; };
    bool storeInCache(const std::wstring &fullPath, HANDLE handle) { return false; };
    bool removeCache(const std::wstring &fullPath) { return false; };
};
