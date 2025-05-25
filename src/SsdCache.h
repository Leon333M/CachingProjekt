// SsdCache.h
#pragma once
#include "CacheInterface.h"
#include <unordered_set>

class SsdCache : public CacheInterface {
private:
    std::unordered_set<std::wstring> cashePfade;

public:
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring &fullPath);
    void Clear();

private:
    std::wstring GetCachePathFromFullPath(const std::wstring &fullPath);
};
