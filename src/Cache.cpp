// Cache.cpp
#include "Cache.h"

Cache::Cache(CacheInterface &ramCache, CacheInterface &ssdCache) : ramCache(ramCache), ssdCache(ssdCache) {
    ramCache.setNextCache(&ssdCache);
}

bool Cache::Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    return ramCache.Read(handle, buffer, length, bytesTransferred, overlapped);
}

bool Cache::Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    bool ret = false;
    bool ret1 = ramCache.Write(handle, buffer, length, bytesTransferred, overlapped);
    bool ret2 = ssdCache.Write(handle, buffer, length, bytesTransferred, overlapped);
    if (ret1 || ret2) {
        ret = true;
    }
    return ret;
}

void Cache::Remove(const std::wstring &fullPath) {
    ramCache.Remove(fullPath);
    ssdCache.Remove(fullPath);
}

void Cache::Clear() {
    ramCache.Clear();
    ssdCache.Clear();
}

void Cache::setNextCache(CacheInterface *cache) {
    ssdCache.setNextCache(cache);
}
