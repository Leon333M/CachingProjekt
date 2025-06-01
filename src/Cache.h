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
    void Clear();
    void setNextCache(CacheInterface *cache);
};
