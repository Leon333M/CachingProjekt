// CacheInterface.cpp
#include "CacheInterface.h"
#include <iostream>
#include <strsafe.h>
#include <winfsp/winfsp.h>

#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

CacheInterface::CacheInterface()
    : pfadHistorie(minZugriffsHaufigkeit, maxPfadHistorie),
      handleHistorie(1, maxHandleHistorie) {}

void CacheInterface::setMaxCacheSize(UINT64 maxCacheSizeInGb) {
    maxCacheSize = maxCacheSizeInGb * 1024 * 1024 * 1024;
}

void CacheInterface::setMinZugriffsHaufigkeit(int minZugriffsHaufigkeit) {
    this->minZugriffsHaufigkeit = minZugriffsHaufigkeit;
    pfadHistorie.setDepth(minZugriffsHaufigkeit);
}

const std::wstring CacheInterface::getCacheTyp() const {
    return cacheTyp;
}

const std::wstring CacheInterface::getCacheName() const {
    return cacheName;
}

bool CacheInterface::read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    // PLOG_DEBUG << L"read : " << fullPath;

    // prufe ob im Cache
    if (!(isCached(fullPath))) {
        if (!shouldHadelCache(handle)) {
            return false;
        }
        if (!shouldCachePath(fullPath)) {
            return false;
        }
        // add zum Cache
        if (!addFile(fullPath, handle)) {
            PLOG_WARNING << "read: Fehler beim Hinzufugen der Datei: " << fullPath;
            return false;
        }

    } else {
        // fullPath ist bereits in cashePfade
        // PLOG_DEBUG << L"read: fullPath vorhanden : " << fullPath;
    }

    // von ReadCash lesen
    BOOL result = readCache(fullPath, handle, buffer, length, bytesTransferred, overlapped);
    if (!result) {
        PLOG_WARNING << L"read: Fehler beim laden von Cashe : " << fullPath;
        return false;
    }
    // PLOG_DEBUG << L"read: Datei von Cashe geladen : " << fullPath;
    return result;
}

bool CacheInterface::write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    removeHandle(handle);
    return false;
}

void CacheInterface::remove(const std::wstring &fullPath) {
    auto it = cashePfade.find(fullPath);
    if (it != cashePfade.end()) {
        // entferne aus Liste
        cashePfade.erase(it);
        // entferne Datei aus Cache
        removeCache(fullPath);
        benachrichtigeListenerAsync();
    } else {
        PLOG_VERBOSE << L"remove: Pfad nicht im Cache gefunden: " << fullPath;
    }
}

void CacheInterface::removeHandle(HANDLE handle) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    // PLOG_DEBUG << L"removeHandle : " << fullPath;
    remove(fullPath);
}

bool CacheInterface::isCached(const std::wstring &fullPath) const {
    return cashePfade.find(fullPath) != cashePfade.end();
}

void CacheInterface::shutdown() {
    clear();
}

std::wstring CacheInterface::pathFromHandle(HANDLE handle) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    return fullPath;
}

void CacheInterface::clear() {
    std::unordered_set<std::wstring> delcashePfade = cashePfade;
    for (std::wstring pfad : delcashePfade) {
        remove(pfad);
    }
}

UINT64 CacheInterface::clear(const UINT64 &size) {
    UINT64 freedSize = 0;
    UINT64 removedSize = 0;
    std::vector<std::wstring> removePfade;
    std::unordered_set<std::wstring> cashePfadeKopie = cashePfade;

    for (const std::wstring &fullPath : cashePfadeKopie) {
        if (removedSize < size) {
            removedSize += sizeFromPath(fullPath);
            removePfade.emplace_back(fullPath);
        } else {
            break;
        }
    }
    for (const std::wstring &fullPath : removePfade) {
        remove(fullPath);
    }
    return removedSize;
}

bool CacheInterface::addFileSize(const UINT64 &fileSize) {
    UINT64 neueSize = currentCacheSize + fileSize;
    if (neueSize > maxCacheSize) {
        if (fileSize > maxCacheSize) {
            PLOG_DEBUG << "addFileSize: Fehler Datei grosser als Cache: " << fileSize << " / " << maxCacheSize;
            return false;
        } else {
            UINT64 diff = neueSize - maxCacheSize;
            UINT64 realClear = clear(diff);
            currentCacheSize += fileSize;
            PLOG_VERBOSE << "addFileSize: " << diff << " Speicher freigemacht " << diff << " / " << realClear;
        }
    } else {
        currentCacheSize = neueSize;
    }
    PLOG_VERBOSE << "addFileSize: Cache: " << currentCacheSize << " / " << maxCacheSize;
    return true;
}

bool CacheInterface::addFile(const std::wstring &fullPath, HANDLE handle) {
    // add Datei zum ReadCash
    PLOG_VERBOSE << L"addFile: " << fullPath;

    // add Dateigroesse
    BY_HANDLE_FILE_INFORMATION handleFileInfo;
    if (!GetFileInformationByHandle(handle, &handleFileInfo)) {
        PLOG_WARNING << "addFile: Fehler bei der handleFileInfo";
        return false;
    }
    UINT64 fileSize = ((UINT64)handleFileInfo.nFileSizeHigh << 32) | (UINT64)handleFileInfo.nFileSizeLow;
    if (!addFileSize(fileSize)) {
        PLOG_WARNING << "addFile: Fehler bei der dateirgoesse: " << fileSize;
        return false;
    }

    // Sicherstellen, dass die Zielverzeichnisse existieren
    // Datei kopieren
    if (!storeInCache(fullPath, handle)) {
        PLOG_WARNING << L"addFile: Fehler beim Kopieren in Cache: " << fullPath;
        return false;
    }

    // Merke das Datei vorhanden
    PLOG_DEBUG << L"addFile: Datei gespeichert: " << fileSize << L" " << fullPath;
    cashePfade.insert(fullPath);
    benachrichtigeListenerAsync();
    return true;
}

UINT64 CacheInterface::sizeFromPath(const std::wstring &Path) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(Path.c_str(), GetFileExInfoStandard, &fileInfo)) {
        PLOG_WARNING << "sizeFromPath: fehler fileInfo nicht gefunden. " << Path;
        return 0;
    }

    ULARGE_INTEGER size;
    size.HighPart = fileInfo.nFileSizeHigh;
    size.LowPart = fileInfo.nFileSizeLow;

    return size.QuadPart;
}

bool CacheInterface::shouldCachePath(const std::wstring &fullPath) {
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

bool CacheInterface::shouldHadelCache(const HANDLE handle) {
    int count = handleHistorie.count(handle);
    // prufe ob im Cache
    if (count < 1) {
        // handle hinzufugen
        handleHistorie.add(handle);
        if (handleHistorie.size() > maxHandleHistorie) {
            handleHistorie.pop_front();
        }
        return true;
    }
    return false;
}

int CacheInterface::countPathInHistory(const std::wstring &fullPath) {
    return pfadHistorie.count(fullPath);
}

void CacheInterface::removePathFromHistory(const std::wstring &fullPath) {
    // Alle Vorkommen aus pfadHistorie entfernen.
    pfadHistorie.remove(fullPath);
}

void CacheInterface::addPathToHistory(const std::wstring &fullPath) {
    // Fuge den Pfad hinten in pfadHistorie ein.
    pfadHistorie.add(fullPath);
    // Wenn recentPaths.size() > maxRecentPaths, entferne das vorderste Element.
    if (pfadHistorie.size() > maxPfadHistorie) {
        pfadHistorie.pop_front();
    }
}
