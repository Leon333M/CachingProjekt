// SsdCache.h
#pragma once
#include "CacheInterface.h"

class SsdCache : public CacheInterface {
public:
    bool Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped){return false;};
    bool Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped){return false;};
    void Remove(const std::wstring& fullPath){};
    void RemoveHandle(HANDLE handle){};
    void Clear(){};
};