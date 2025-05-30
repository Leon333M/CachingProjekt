// CacheInterface.h
#pragma once
#include <deque>
#include <string>
#include <unordered_set>
#include <windows.h>

class CacheInterface {
protected:
    UINT64 maxCacheSize = 8ULL * 1024 * 1024 * 1024; // 8 GB
    UINT64 currentCacheSize = 0;
    std::unordered_set<std::wstring> cashePfade;
    std::deque<std::wstring> pfadHistorie;
    int minZugriffsHaufigkeit = 2;
    const int maxPfadHistorie = 64;
    std::deque<HANDLE> handleHistorie;
    const int maxHandleHistorie = 8;

public:
    virtual ~CacheInterface() = default;
    virtual bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) = 0;
    virtual bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) = 0;
    virtual void Remove(const std::wstring &fullPath) = 0;
    void RemoveHandle(HANDLE handle);
    virtual void Clear();
    void setMaxCacheSize(UINT64 maxCacheSizeInGb);
    void setMinZugriffsHaufigkeit(int minZugriffsHaufigkeit);

    bool ShouldHadelCache(const HANDLE handle);
};
