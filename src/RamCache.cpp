// RamCache.cpp
#include "RamCache.h"
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

bool RamCache::storeInRam(const std::wstring &fullPath) {
    // Datei im Binarmodus oeffnen und an das Ende springen
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::wcerr << L"storeInRam: Fehler beim Oeffnen der Datei: " << fullPath << std::endl;
        return false;
    }

    // Dateigroesse ermitteln und zum Anfang springen
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Datei einlesen
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        std::wcerr << L"storeInRam: Fehler beim Lesen der Datei: " << fileSize << L" " << fullPath << std::endl;
        return false;
    }

    // Bloecke in Cache eintragen
    ramCache[fullPath] = std::move(buffer);

    // std::wcout << L"storeInRam: Lesen der Datei: " << fileSize << L" " << fullPath << std::endl;

    return true;
}

bool RamCache::readFromRam(const std::wstring &originalPath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    // Offset aus OVERLAPPED-Struktur berechnen
    UINT64 requestOffset = (static_cast<UINT64>(overlapped->OffsetHigh) << 32) | overlapped->Offset;

    // Datei im Cache suchen
    auto itFile = ramCache.find(originalPath);
    if (itFile == ramCache.end()) {
        std::wcerr << L"readFromRam: Fehler: Datei nicht im Cache: " << originalPath << std::endl;
        return false;
    }

    const std::vector<char> &block = itFile->second;
    DWORD dataLength = length;

    // Pruefen ob der Offset innerhalb des Blocks liegt
    if (requestOffset + dataLength > block.size()) {
        if (requestOffset < block.size()) {
            // Gehe davon aus, dass die Anfrage uber das Dateilimit geht, und korrigiere die Anfrage.
            dataLength = block.size() - requestOffset;
        } else {
            std::wcerr << L"readFromRam: Fehler: Offset ausserhalb der Blockgroesse. " << originalPath << std::endl;
            std::wcout << L"readFromRam: requestOffset length block.size(): "
                       << requestOffset << L" " << dataLength << L" " << requestOffset + dataLength << L" " << block.size() << std::endl;
            return false;
        }
    }

    // Daten aus Block in Zielpuffer kopieren
    memcpy(buffer, block.data() + requestOffset, dataLength);
    *bytesTransferred = dataLength;

    return true;
}

bool RamCache::readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    return readFromRam(fullPath, buffer, length, bytesTransferred, overlapped);
}

bool RamCache::storeInCache(const std::wstring &fullPath, HANDLE handle) {
    return storeInRam(fullPath);
}
