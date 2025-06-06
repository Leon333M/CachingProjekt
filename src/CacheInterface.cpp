// CacheInterface.cpp
#include "CacheInterface.h"
#include <iostream>
#include <strsafe.h>
#include <winfsp/winfsp.h>

#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

CacheInterface::CacheInterface()
    : pfadHistorie(minZugriffsHaufigkeit, maxPfadHistorie),
      handleHistorie(1, maxHandleHistorie) {}

void CacheInterface::RemoveHandle(HANDLE handle) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    // PLOG_DEBUG << L"RemoveHandle : " << fullPath;
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

UINT64 CacheInterface::Clear(const UINT64 &size) {
    UINT64 freedSize = 0;
    UINT64 removedSize = 0;
    std::vector<std::wstring> removePfade;
    std::unordered_set<std::wstring> cashePfadeKopie = cashePfade;

    for (const std::wstring &fullPath : cashePfadeKopie) {
        if (removedSize < size) {
            removedSize += SizeFromPath(fullPath);
            removePfade.emplace_back(fullPath);
        } else {
            break;
        }
    }
    for (const std::wstring &fullPath : removePfade) {
        Remove(fullPath);
    }
    return removedSize;
}

UINT64 CacheInterface::SizeFromPath(const std::wstring &Path) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(Path.c_str(), GetFileExInfoStandard, &fileInfo)) {
        PLOG_ERROR << "SizeFromPath: fehler fileInfo nicht gefunden. " << Path;
        return 0;
    }

    ULARGE_INTEGER size;
    size.HighPart = fileInfo.nFileSizeHigh;
    size.LowPart = fileInfo.nFileSizeLow;

    return size.QuadPart;
}

void CacheInterface::setMinZugriffsHaufigkeit(int minZugriffsHaufigkeit) {
    this->minZugriffsHaufigkeit = minZugriffsHaufigkeit;
    pfadHistorie.setDepth(minZugriffsHaufigkeit);
}

void CacheInterface::setNextCache(CacheInterface *cache) {
    nextCache = cache;
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

bool CacheInterface::readNextCache(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    if (nextCache != nullptr) {
        return nextCache->Read(handle, buffer, length, bytesTransferred, overlapped);
    }
    return false;
}

bool CacheInterface::Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    // PLOG_DEBUG << L"Read : " << fullPath;

    // prufe ob im Cache
    if (!(cashePfade.find(fullPath) != cashePfade.end())) {
        if (readNextCache(handle, buffer, length, bytesTransferred, overlapped)) {
            return true;
        }
        if (!ShouldHadelCache(handle)) {
            return false;
        }
        if (!ShouldCachePath(fullPath)) {
            return false;
        }
        // add zum Cache
        if (!AddFile(fullPath, handle)) {
            PLOG_ERROR << "Read: Fehler beim hinzufugen der Datei: " << fullPath;
            return false;
        }

    } else {
        // fullPath ist bereits in cashePfade
        // PLOG_DEBUG << L"Read: fullPath vorhanden : " << fullPath;
    }

    // von ReadCash lesen
    BOOL result = readCache(fullPath, handle, buffer, length, bytesTransferred, overlapped);
    if (!result) {
        PLOG_ERROR << L"Read: Fehler beim laden von Cashe : " << fullPath;
        return false;
    }
    // PLOG_DEBUG << L"Read: Datei von Cashe geladen : " << fullPath;
    return result;
}

bool CacheInterface::Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    RemoveHandle(handle);
    return false;
}

bool CacheInterface::AddFileSize(const UINT64 &fileSize) {
    UINT64 neueSize = currentCacheSize + fileSize;
    if (neueSize > maxCacheSize) {
        if (fileSize > maxCacheSize) {
            PLOG_DEBUG << "AddFileSize: Fehler Datei grosser als Cache: " << fileSize << " / " << maxCacheSize;
            return false;
        } else {
            UINT64 diff = neueSize - maxCacheSize;
            UINT64 realClear = Clear(diff);
            currentCacheSize += fileSize;
            // PLOG_DEBUG << "AddFileSize: " << diff << " Speicher freigemacht";
            // PLOG_DEBUG << diff << " / " << realClear;
        }
    } else {
        currentCacheSize = neueSize;
    }
    // PLOG_DEBUG << "AddFileSize: Cache: " << currentCacheSize << " / " << maxCacheSize;
    return true;
}

bool CacheInterface::AddFile(const std::wstring &fullPath, HANDLE handle) {
    // add Datei zum ReadCash
    // PLOG_DEBUG << L"AddFile: fullPath NICHT vorhanden, kopiere: " << fullPath << L" zu : " << cachePath;

    // add Dateigroesse
    BY_HANDLE_FILE_INFORMATION handleFileInfo;
    if (!GetFileInformationByHandle(handle, &handleFileInfo)) {
        PLOG_ERROR << "AddFile: Fehler bei der handleFileInfo";
        return false;
    }
    UINT64 fileSize = ((UINT64)handleFileInfo.nFileSizeHigh << 32) | (UINT64)handleFileInfo.nFileSizeLow;
    if (!AddFileSize(fileSize)) {
        PLOG_ERROR << "AddFile: Fehler bei der dateirgoesse: " << fileSize;
        return false;
    }

    // Sicherstellen, dass die Zielverzeichnisse existieren
    // Datei kopieren
    if (!storeInCache(fullPath, handle)) {
        PLOG_ERROR << L"AddFile: Fehler beim Kopieren in Cache: " << fullPath;
        return false;
    }

    // Merke das Datei vorhanden
    PLOG_DEBUG << L"AddFile: Datei gespeichert: " << fileSize << L" " << fullPath;
    cashePfade.insert(fullPath);
    return true;
};

void CacheInterface::Remove(const std::wstring &fullPath) {
    auto it = cashePfade.find(fullPath);
    if (it != cashePfade.end()) {
        // entferne aus Liste
        cashePfade.erase(it);
        // entferne Datei aus Cache
        removeCache(fullPath);
    } else {
        // PLOG_DEBUG << L"Remove: Pfad nicht im Cache gefunden: " << fullPath;
    }
};