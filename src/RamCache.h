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
    void Remove(const std::wstring &fullPath);

private:
    bool readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
    bool storeInCache(const std::wstring &fullPath, HANDLE handle);

    // eigene Funktionen
    bool storeInRam(const std::wstring &fullPath);
    bool readFromRam(const std::wstring &originalPath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped);
};
