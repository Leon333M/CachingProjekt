// CacheInterface.cpp
#include "CacheInterface.h"
#include <iostream>
#include <winfsp/winfsp.h>
#include <strsafe.h>

#define FULLPATH_SIZE                   (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

void CacheInterface::RemoveHandle(HANDLE handle){
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    std::wcout << L"RemoveHandle : "<< fullPath << std::endl;
    Remove(fullPath);
}