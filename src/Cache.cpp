// Cache.cpp
#include "Cache.h"

Cache::Cache(std::wstring name, CacheInterface &ramCache, CacheInterface &ssdCache) : ramCache(ramCache), ssdCache(ssdCache) {
    cacheName = name;
    cacheTyp = L"Cache";
    maxCacheSize = 0;
}

bool Cache::read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    if (!ramCache.isCached(pathFromHandle(handle))) {
        if (!ssdCache.read(handle, buffer, length, bytesTransferred, overlapped)) {
            return ramCache.read(handle, buffer, length, bytesTransferred, overlapped);
        }
    } else {
        return ramCache.read(handle, buffer, length, bytesTransferred, overlapped);
    }
    return true;
}

bool Cache::write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    bool ret = false;
    bool ret1 = ramCache.write(handle, buffer, length, bytesTransferred, overlapped);
    bool ret2 = ssdCache.write(handle, buffer, length, bytesTransferred, overlapped);
    if (ret1 || ret2) {
        ret = true;
    }
    return ret;
}

void Cache::remove(const std::wstring &fullPath) {
    ramCache.remove(fullPath);
    ssdCache.remove(fullPath);
}

bool Cache::isCached(const std::wstring &fullPath) const {
    if (!ramCache.isCached(fullPath)) {
        return ssdCache.isCached(fullPath);
    }
    return true;
}

CacheInterface &Cache::getRamCache() {
    return ramCache;
}
CacheInterface &Cache::getSsdCache() {
    return ssdCache;
}
