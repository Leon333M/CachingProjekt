// RamCache.cpp
#include "RamCache.h"

RamCache::RamCache(UINT64 maxCacheSizeInGb) {};
bool RamCache::Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) { return false; };
bool RamCache::Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) { return false; };
void RamCache::Remove(const std::wstring &fullPath) {};
void RamCache::Clear() {};
