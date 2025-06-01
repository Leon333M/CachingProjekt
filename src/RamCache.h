// RamCache.h
#pragma once
#include "CacheInterface.h"
#include <map>
#include <unordered_map>

class RamCache : public CacheInterface {
private:
    std::unordered_map<std::wstring, std::vector<char>> ramCache;

public:
    RamCache(UINT64 maxCacheSizeInGb, int minZugriffsHaufigkeit = 2);
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    void Remove(const std::wstring &fullPath);

private:
    bool AddFile(std::wstring &originalPath, HANDLE handle, WCHAR fullPath[1284]);
    bool AddFileSize(const UINT64 &fileSize);
    bool storeInRam(const std::wstring &fullPath);
    bool readFromRam(const std::wstring &originalPath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
};
