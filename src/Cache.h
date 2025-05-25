// Cache.h
#pragma once
#include "SsdCache.h"
#include "RamCache.h"

class Cache : public CacheInterface {
private:
    SsdCache ssdCache;
    RamCache ramCache;

public:
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring& fullPath);
    void RemoveHandle(HANDLE handle);
    void Clear();
};
