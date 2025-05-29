// Cache.cpp
#include "Cache.h"

Cache::Cache(std::wstring ssdCacheValue) : ssdCache(ssdCacheValue), ramCache() {}

bool Cache::Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    bool ret = true;
    if (!ramCache.Read(handle, buffer, length, bytesTransferred, overlapped)) {
        if (!ssdCache.Read(handle, buffer, length, bytesTransferred, overlapped)) {
            ret = false;
        }
    }
    return ret;
}

bool Cache::Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    ramCache.Write(handle, buffer, length, bytesTransferred, overlapped);
    ssdCache.Write(handle, buffer, length, bytesTransferred, overlapped);
    return false;
}

void Cache::Remove(const std::wstring &fullPath) {
    ramCache.Remove(fullPath);
    ssdCache.Remove(fullPath);
}

void Cache::Clear() {
    ramCache.Clear();
    ssdCache.Clear();
}
