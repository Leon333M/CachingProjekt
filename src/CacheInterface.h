// CacheInterface.h
#pragma once
#include <string>
#include <windows.h>

class CacheInterface {
protected:
    UINT64 maxCacheSize = 8ULL * 1024 * 1024 * 1024; // 8 GB
    UINT64 currentCacheSize = 0;

public:
    virtual ~CacheInterface() = default;

    // Reine virtuelle Methoden
    virtual bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) = 0;
    virtual bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) = 0;
    virtual void Remove(const std::wstring &fullPath) = 0;
    void RemoveHandle(HANDLE handle);
    virtual void Clear() = 0;
};
