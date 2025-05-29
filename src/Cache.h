// Cache.h
#pragma once
#include "RamCache.h"
#include "SsdCache.h"

class Cache : public CacheInterface {
private:
    SsdCache &ssdCache;
    RamCache &ramCache;

public:
    Cache(SsdCache &ssdCache, RamCache &ramCache);
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring &fullPath);
    void Clear();
};
