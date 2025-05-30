// RamCache.cpp
#include "RamCache.h"
#include "CacheInterface.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <strsafe.h>
#include <winfsp/winfsp.h>
#ifdef min
#undef min
#endif
#include <algorithm>

#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

RamCache::RamCache(UINT64 maxCacheSizeInGb, int minZugriffsHaufigkeit) {
    setMaxCacheSize(maxCacheSizeInGb);
    setMinZugriffsHaufigkeit(minZugriffsHaufigkeit);
}

bool RamCache::Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    // std::wcout << L"Read : " << fullPath << std::endl;
    std::wstring originalPath = fullPath;
    originalPath = originalPath.substr(4);

    // prufe ob im Cache
    if (!(cashePfade.find(fullPath) != cashePfade.end())) {
        if (!ShouldHadelCache(handle)) {
            return false;
        }
        if (!ShouldCachePath(fullPath)) {
            return false;
        }
        // add zum Cache
        if (!AddFile(originalPath, handle, fullPath)) {
            std::wcout << "Read: Fehler beim hinzufugen der Datei: " << fullPath << std::endl;
            return false;
        }
    } else {
        // fullPath ist bereits in cashePfade
        // std::wcout << L"Read: fullPath vorhanden : " << fullPath << std::endl;
    }

    // von ReadCash lesen
    BOOL result = readFromRam(originalPath, buffer, length, bytesTransferred, overlapped);
    if (!result) {
        std::wcout << L"Read: Fehler beim laden von RamCashe : " << originalPath << std::endl;
        return false;
    }
    return result;
}

bool RamCache::Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    RemoveHandle(handle);
    return false;
};

void RamCache::Remove(const std::wstring &fullPath) {
    auto it = cashePfade.find(fullPath);
    if (it != cashePfade.end()) {
        // entferne aus Liste
        cashePfade.erase(it);
        // entferne Datei aus Cache
        auto itCache = ramCache.find(fullPath);
        if (itCache != ramCache.end()) {
            ramCache.erase(itCache);
            std::wcout << L"Remove: Datei entfernt aus Cache: " << fullPath << std::endl;
        } else {
            // std::wcout << L"Remove: Datei nicht im Cache: " << fullPath << std::endl;
        }
    } else {
        // std::wcout << L"Remove: Pfad nicht im Cache gefunden: " << fullPath << std::endl;
    }
};

// private Funktioen

bool RamCache::AddFile(std::wstring &originalPath, HANDLE handle, WCHAR fullPath[1284]) {
    // add Datei zum ReadCash
    // std::wcout << L"AddFile: fullPath NICHT vorhanden, kopiere: " << originalPath << L" zu : " << cachePath << std::endl;

    // add Dateigroesse
    BY_HANDLE_FILE_INFORMATION handleFileInfo;
    if (!GetFileInformationByHandle(handle, &handleFileInfo)) {
        std::cout << "AddFile: Fehler bei der handleFileInfo" << std::endl;
        return FspNtStatusFromWin32(GetLastError());
    }
    UINT64 FileSize = ((UINT64)handleFileInfo.nFileSizeHigh << 32) | (UINT64)handleFileInfo.nFileSizeLow;
    if (!AddFileSize(FileSize)) {
        std::cout << "AddFile: Fehler bei der dateirgoesse: " << FileSize << std::endl;
        return false;
    }

    // Sicherstellen, dass die Zielverzeichnisse existieren
    // Datei kopieren
    if (!storeInRam(originalPath)) {
        std::wcout << L"AddFile: Fehler beim Kopieren in Cache: " << originalPath << std::endl;
        return false;
    }

    // Merke das Datei vorhanden
    std::wcout << L"AddFile: fullPath insert : " << fullPath << std::endl;
    cashePfade.insert(fullPath);
    return true;
};

bool RamCache::AddFileSize(const UINT64 &fileSize) {
    UINT64 neueSize = currentCacheSize + fileSize;
    if (neueSize > maxCacheSize) {
        if (fileSize > maxCacheSize) {
            std::cout << "AddFileSize: Fehler Datei grosser als Cache: " << fileSize << " / " << maxCacheSize << std::endl;
            return false;
        } else {
            UINT64 diff = neueSize - maxCacheSize;
            UINT64 realClear = Clear(diff);
            currentCacheSize += fileSize;
            // std::cout << "AddFileSize: " << diff << " Speicher freigemacht" << std::endl;
            // std::cout << diff << " / " << realClear << std::endl;
        }
    } else {
        currentCacheSize = neueSize;
    }
    // std::cout << "AddFileSize: Cache: " << currentCacheSize << " / " << maxCacheSize << std::endl;
    return true;
}

UINT64 RamCache::Clear(const UINT64 &size) {
    UINT64 freedSize = 0;
    UINT64 removedSize = 0;
    std::vector<std::wstring> removePfade;

    for (const std::wstring fullPath : cashePfade) {
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

UINT64 RamCache::SizeFromPath(const std::wstring &Path) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(Path.c_str(), GetFileExInfoStandard, &fileInfo)) {
        return 0; // oder ggf. Fehlerbehandlung
    }

    ULARGE_INTEGER size;
    size.HighPart = fileInfo.nFileSizeHigh;
    size.LowPart = fileInfo.nFileSizeLow;

    return size.QuadPart;
}

bool RamCache::ShouldCachePath(const std::wstring &fullPath) {
    // Zahle, wie oft der Pfad in pfadHistorie bereits vorkommt.
    int count = std::count(pfadHistorie.begin(), pfadHistorie.end(), fullPath);
    // Wenn ≥ minZugriffsHaufigkeit z.B. 2
    if (count >= minZugriffsHaufigkeit) {
        // - Alle Vorkommen aus pfadHistorie entfernen.
        pfadHistorie.erase(std::remove(pfadHistorie.begin(), pfadHistorie.end(), fullPath), pfadHistorie.end());
        // - Gib true zuruck (→ jetzt cachen).
        return true;
    } else { // Sonst:
        // - Fuge den Pfad hinten in pfadHistorie ein.
        pfadHistorie.push_back(fullPath);
        // - Wenn recentPaths.size() > maxRecentPaths, entferne das vorderste Element.
        if (pfadHistorie.size() > maxPfadHistorie) {
            pfadHistorie.pop_front();
        }
        // - Gib false zuruck (→ noch nicht cachen).
        return false;
    }
}
bool RamCache::storeInRam(const std::wstring &fullPath) {
    const UINT64 BLOCK_SIZE = 4ULL * 1024 * 1024; // 4 MB pro Block

    // Datei im Binarmodus oeffnen und an das Ende springen
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::wcerr << L"[RAM-Cache] Fehler beim Oeffnen der Datei: " << fullPath << std::endl;
        return false;
    }

    // Dateigroesse ermitteln und zum Anfang springen
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::map<UINT64, std::vector<char>> blocks;
    UINT64 offset = 0;
    int blockCount = 0;

    // Datei blockweise einlesen
    while (fileSize > 0) {
        std::vector<char> buffer(BLOCK_SIZE);
        std::streamsize toRead = std::min<std::streamsize>(BLOCK_SIZE, fileSize);

        if (!file.read(buffer.data(), toRead)) {
            std::wcerr << L"[RAM-Cache] Fehler beim Lesen der Datei bei Offset " << offset << std::endl;
            return false;
        }

        buffer.resize(toRead); // Nur tatsaechlich gelesene Bytes speichern
        blocks[offset] = std::move(buffer);

        offset += BLOCK_SIZE;
        fileSize -= toRead;
        blockCount++;
    }

    // Bloecke in Cache eintragen
    ramCache[fullPath] = std::move(blocks);

    return true;
}

bool RamCache::readFromRam(const std::wstring &originalPath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    const UINT64 BLOCK_SIZE = 4ULL * 1024 * 1024; // 4 MB wie beim Schreiben

    // Offset aus OVERLAPPED-Struktur berechnen
    UINT64 requestOffset = (static_cast<UINT64>(overlapped->OffsetHigh) << 32) | overlapped->Offset;

    // Datei im Cache suchen
    auto itFile = ramCache.find(originalPath);
    if (itFile == ramCache.end()) {
        std::wcerr << L"[RAM-Cache] Fehler: Datei nicht im Cache: " << originalPath << std::endl;
        return false;
    }

    // Block-Offset und Position innerhalb des Blocks berechnen
    UINT64 blockOffset = (requestOffset / BLOCK_SIZE) * BLOCK_SIZE;
    UINT64 withinBlockOffset = requestOffset % BLOCK_SIZE;

    // Gesuchten Block im Cache suchen
    auto itBlock = itFile->second.find(blockOffset);
    if (itBlock == itFile->second.end()) {
        std::wcerr << L"[RAM-Cache] Fehler: Kein Block gefunden fuer Offset: " << requestOffset
                   << L" (Block-Offset: " << blockOffset << L")" << std::endl;
        return false;
    }

    const std::vector<char> &block = itBlock->second;

    // Pruefen ob der Offset innerhalb des Blocks liegt
    if (withinBlockOffset >= block.size()) {
        std::wcerr << L"[RAM-Cache] Fehler: Innerhalb des Blocks liegt Offset ausserhalb der Blockgroesse." << std::endl;
        return false;
    }

    // Anzahl der zu kopierenden Bytes berechnen
    DWORD copySize = std::min<DWORD>(length, static_cast<DWORD>(block.size() - withinBlockOffset));

    // Daten aus Block in Zielpuffer kopieren
    memcpy(buffer, block.data() + withinBlockOffset, copySize);
    *bytesTransferred = copySize;

    return true;
}
