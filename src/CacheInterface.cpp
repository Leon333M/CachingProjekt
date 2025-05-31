// CacheInterface.cpp
#include "CacheInterface.h"
#include <iostream>
#include <strsafe.h>
#include <winfsp/winfsp.h>

#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

std::shared_mutex pfadHistorieMutex; // sperre fur Thread sicherheits

void CacheInterface::RemoveHandle(HANDLE handle) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    // std::wcout << L"RemoveHandle : " << fullPath << std::endl;
    Remove(fullPath);
}

void CacheInterface::setMaxCacheSize(UINT64 maxCacheSizeInGb) {
    maxCacheSize = maxCacheSizeInGb * 1024 * 1024 * 1024;
}

void CacheInterface::Clear() {
    std::unordered_set<std::wstring> delcashePfade = cashePfade;
    for (std::wstring pfad : delcashePfade) {
        Remove(pfad);
    }
}

void CacheInterface::setMinZugriffsHaufigkeit(int minZugriffsHaufigkeit) {
    this->minZugriffsHaufigkeit = minZugriffsHaufigkeit;
}

bool CacheInterface::ShouldCachePath(const std::wstring &fullPath) {
    // Zahle, wie oft der Pfad in pfadHistorie bereits vorkommt.
    int count = countPathInHistory(fullPath);
    // Wenn ≥ minZugriffsHaufigkeit z.B. 2
    if (count >= minZugriffsHaufigkeit) {
        // - Alle Vorkommen aus pfadHistorie entfernen.
        removePathFromHistory(fullPath);
        // - Gib true zuruck (→ jetzt cachen).
        return true;
    } else { // Sonst:
        addPathToHistory(fullPath);
        // - Gib false zuruck (→ noch nicht cachen).
        return false;
    }
}

bool CacheInterface::ShouldHadelCache(const HANDLE handle) {
    int count = std::count(handleHistorie.begin(), handleHistorie.end(), handle);
    // prufe ob im Cache
    if (count < 1) {
        // handle hinzufugen
        handleHistorie.push_back(handle);
        if (handleHistorie.size() > maxHandleHistorie) {
            handleHistorie.pop_front();
        }
        return true;
    }
    return false;
}

int CacheInterface::countPathInHistory(const std::wstring &fullPath) {
    std::shared_lock<std::shared_mutex> sperre(pfadHistorieMutex); // fur Thread sicherheit
    return std::count(pfadHistorie.begin(), pfadHistorie.end(), fullPath);
}

void CacheInterface::removePathFromHistory(const std::wstring &fullPath) {
    std::lock_guard<std::shared_mutex> sperre(pfadHistorieMutex); // fur Thread sicherheit
    // - Alle Vorkommen aus pfadHistorie entfernen.
    pfadHistorie.erase(std::remove(pfadHistorie.begin(), pfadHistorie.end(), fullPath), pfadHistorie.end());
}

void CacheInterface::addPathToHistory(const std::wstring &fullPath) {
    std::lock_guard<std::shared_mutex> sperre(pfadHistorieMutex); // fur Thread sicherheit
    //  - Fuge den Pfad hinten in pfadHistorie ein.
    pfadHistorie.push_back(fullPath);
    // - Wenn recentPaths.size() > maxRecentPaths, entferne das vorderste Element.
    if (pfadHistorie.size() > maxPfadHistorie) {
        pfadHistorie.pop_front();
    }
}
