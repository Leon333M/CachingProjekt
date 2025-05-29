// RamCache.h
#pragma once
#include "CacheInterface.h"

class RamCache : public CacheInterface {
public:
    RamCache(UINT64 maxCacheSizeInGb);
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring &fullPath);
    void Clear();
};
