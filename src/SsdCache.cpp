// SsdCache.cpp
#include "SsdCache.h"
#include <filesystem>
#include <iostream>
#include <strsafe.h>
#include <winfsp/winfsp.h>

#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

SsdCache::SsdCache(std::wstring name, std::wstring ssdCacheValue, UINT64 maxCacheSizeInGb, int minZugriffsHaufigkeit) {
    cacheName = name;
    cacheTyp = L"SsdCache";
    cacheVolume = ssdCacheValue;
    cacheStammVerzeichnis = cacheVolume + L"/Cashe/" + cacheName + L"/";
    setMaxCacheSize(maxCacheSizeInGb);
    setMinZugriffsHaufigkeit(minZugriffsHaufigkeit);
    clearCacheVerzeichnis();
}

bool SsdCache::readCache(const std::wstring &fullPath, HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    std::wstring cachePath = getCachePathFromFullPath(fullPath);
    return readSsdCache(cachePath, buffer, length, bytesTransferred, overlapped);
}

bool SsdCache::storeInCache(const std::wstring &fullPath, HANDLE handle) {
    std::wstring cachePath = getCachePathFromFullPath(fullPath);
    return storeInSsdCache(fullPath, cachePath);
}

bool SsdCache::readSsdCache(const std::wstring &cachePath, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    HANDLE cacheHandle = CreateFileW(cachePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (cacheHandle == INVALID_HANDLE_VALUE) {
        PLOG_WARNING << L"readSsdCache: Fehler beim Offnen der Cache-Datei: " << cachePath;
        return false;
    }
    BOOL result = ReadFile(cacheHandle, buffer, length, bytesTransferred, overlapped);
    CloseHandle(cacheHandle);
    if (!result) {
        PLOG_WARNING << L"readSsdCache: Fehler beim laden von Cashe : " << cachePath;
        return false;
    }
    PLOG_VERBOSE << L"readSsdCache: Datei von Cashe geladen : " << cachePath;
    return result;
}

// private Funktioen
bool SsdCache::removeCache(const std::wstring &fullPath) {
    std::wstring cachePath = getCachePathFromFullPath(fullPath);
    if (std::filesystem::exists(cachePath)) {
        UINT64 size = sizeFromPath(cachePath);
        try {
            std::filesystem::remove(cachePath);
            PLOG_DEBUG << L"remove: Datei im Cache geloscht: " << cachePath;
            currentCacheSize -= size;
            return true;
        } catch (const std::filesystem::filesystem_error &e) {
            PLOG_WARNING << L"remove: Fehler beim Loschen der Datei im Cache: ";
        }
    } else {
        PLOG_DEBUG << L"remove: Datei existiert nicht im Cache: " << cachePath;
    }
    return false;
}

// Hilfsfunktion, um den Cache-Pfad zu extrahieren
std::wstring SsdCache::getCachePathFromFullPath(const std::wstring &fullPath) {
    // Der vollstandige Pfad wird bearbeitet, um den relativen Cache-Pfad zu erhalten
    std::wstring originalPath = fullPath;
    originalPath = originalPath.substr(4); // Entferne \\?\ 
    std::wstring originalPathForCashe = originalPath;
    originalPathForCashe.erase(std::remove(originalPathForCashe.begin(), originalPathForCashe.end(), L':'), originalPathForCashe.end());
    // Der Cache-Pfad wird erstellt
    std::wstring cachePath = cacheStammVerzeichnis + originalPathForCashe;
    std::replace(cachePath.begin(), cachePath.end(), L'\\', L'/');

    return cachePath;
}

bool SsdCache::storeInSsdCache(const std::wstring &originalPath, std::wstring &cachePath) {
    PLOG_VERBOSE << L"storeInSsdCache wird aufegrufen " << originalPath << L" " << cachePath;
    // Sicherstellen, dass die Zielverzeichnisse existieren
    std::filesystem::create_directories(std::filesystem::path(cachePath).parent_path());
    // Datei kopieren
    if (!CopyFileW(originalPath.c_str(), cachePath.c_str(), FALSE)) {
        PLOG_WARNING << L"storeInSsdCache: Fehler beim Kopieren in Cache: " << originalPath;
        return false;
    }
    return true;
};

void SsdCache::clearCacheVerzeichnis() {
    try {
        // Uberprufe, ob das Verzeichnis existiert
        if (std::filesystem::exists(cacheStammVerzeichnis) && std::filesystem::is_directory(cacheStammVerzeichnis)) {
            // Losche rekursiv alle Dateien und Unterverzeichnisse
            for (const auto &entry : std::filesystem::directory_iterator(cacheStammVerzeichnis)) {
                std::filesystem::remove_all(entry); // Entfernt die Datei/Ordner
            }
        }
    } catch (const std::exception &e) {
        PLOG_WARNING << "Fehler beim Loschen des Cache-Verzeichnisses: " << e.what();
    }
}
