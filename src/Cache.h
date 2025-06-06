// Cache.h
#pragma once
#include "RamCache.h"
#include "SsdCache.h"

class Cache : public CacheInterface {
private:
    CacheInterface &ssdCache;
    CacheInterface &ramCache;

public:
    Cache(CacheInterface &ramCache, CacheInterface &ssdCache);
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring &fullPath);
    void setNextCache(CacheInterface *cache);

private:
    // eigene Funktionen
    // ungenutze Funktionen
    bool readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) { return false; };
    bool storeInCache(const std::wstring &fullPath, HANDLE handle) { return false; };
    bool removeCache(const std::wstring &fullPath) { return false; };
};
