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

// private Funktioen

bool RamCache::removeCache(const std::wstring &fullPath) {
    auto itCache = ramCache.find(fullPath);
    if (itCache != ramCache.end()) {
        const std::vector<char> &block = itCache->second;
        UINT64 size = block.size();
        ramCache.erase(itCache);
        currentCacheSize -= size;
        PLOG_DEBUG << L"remove: Datei entfernt aus Cache: " << fullPath;
        return true;
    } else {
        PLOG_WARNING << L"remove: Datei nicht im Cache: " << fullPath;
        return false;
    }
}

bool RamCache::storeInRam(const std::wstring &fullPath) {
    // Datei im Binarmodus oeffnen und an das Ende springen
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file) {
        PLOG_WARNING << L"storeInRam: Fehler beim Oeffnen der Datei: " << fullPath;
        return false;
    }

    // Dateigroesse ermitteln und zum Anfang springen
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Datei einlesen
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        PLOG_WARNING << L"storeInRam: Fehler beim Lesen der Datei: " << fileSize << L" " << fullPath;
        return false;
    }

    // Bloecke in Cache eintragen
    ramCache[fullPath] = std::move(buffer);

    PLOG_VERBOSE << L"storeInRam: Lesen der Datei: " << fileSize << L" " << fullPath;

    return true;
}

bool RamCache::readFromRam(const std::wstring &originalPath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    // Offset aus OVERLAPPED-Struktur berechnen
    UINT64 requestOffset = (static_cast<UINT64>(overlapped->OffsetHigh) << 32) | overlapped->Offset;

    // Datei im Cache suchen
    auto itFile = ramCache.find(originalPath);
    if (itFile == ramCache.end()) {
        PLOG_WARNING << L"readFromRam: Fehler: Datei nicht im Cache: " << originalPath;
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
            PLOG_VERBOSE << L"readFromRam: Fehler: Offset ausserhalb der Blockgroesse. " << originalPath;
            PLOG_VERBOSE << L"readFromRam: requestOffset length block.size(): "
                         << requestOffset << L" " << dataLength << L" " << requestOffset + dataLength << L" " << block.size();
            bytesTransferred = 0;
            return true;
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
