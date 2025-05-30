// SsdCache.cpp
#include "SsdCache.h"
#include <filesystem>
#include <iostream>
#include <strsafe.h>
#include <winfsp/winfsp.h>

#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

SsdCache::SsdCache(std::wstring ssdCacheValue, UINT64 maxCacheSizeInGb, int minZugriffsHaufigkeit) {
    cacheVolume = ssdCacheValue;
    cacheStammVerzeichnis = cacheVolume + L"/Cashe/";
    setMaxCacheSize(maxCacheSizeInGb);
    setMinZugriffsHaufigkeit(minZugriffsHaufigkeit);
}

bool SsdCache::Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    WCHAR fullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, fullPath, FULLPATH_SIZE - 1, 0);
    // std::wcout << L"Read : " << fullPath << std::endl;
    std::wstring originalPath = fullPath;
    originalPath = originalPath.substr(4);
    std::wstring cachePath = GetCachePathFromFullPath(fullPath);

    // prufe ob im Cache
    if (!(cashePfade.find(fullPath) != cashePfade.end())) {
        if (!ShouldCachePath(fullPath)) {
            return false;
        }
        // add zum Cache
        if (!AddFile(originalPath, cachePath, handle, fullPath)) {
            std::wcout << "Read: Fehler beim hinzufugen der Datei: " << fullPath << std::endl;
            return false;
        }
    } else {
        // fullPath ist bereits in cashePfade
        // std::wcout << L"Read: fullPath vorhanden : " << fullPath << std::endl;
    }

    // Jetzt von E:/Cashe/... lesen
    // von ReadCash lesen
    HANDLE cacheHandle = CreateFileW(cachePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (cacheHandle == INVALID_HANDLE_VALUE) {
        std::wcout << L"Read: Fehler beim Offnen der Cache-Datei: " << cachePath << std::endl;
        return false;
    }
    BOOL result = ReadFile(cacheHandle, buffer, length, bytesTransferred, overlapped);
    CloseHandle(cacheHandle);
    if (!result) {
        std::wcout << L"Read: Fehler beim laden von Cashe : " << cachePath << std::endl;
        return false;
    }
    // std::wcout << L"ReadCash: fullPath vorhanden und von Cashe geladen : " << cachePath << std::endl;
    // result = false; // erstmal, entferen ich spater
    return result;
}

bool SsdCache::Write(HANDLE handle, LPCVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    RemoveHandle(handle);
    return false;
};

void SsdCache::Remove(const std::wstring &fullPath) {
    auto it = cashePfade.find(fullPath);
    if (it != cashePfade.end()) {
        // Hier konnen spater weitere Aktionen hinzugefugt werden
        // entferne aus Liste
        cashePfade.erase(it);
        // entferne Datei aus Cache
        std::wstring cachePath = GetCachePathFromFullPath(fullPath);
        if (std::filesystem::exists(cachePath)) {
            UINT64 size = SizeFromPath(cachePath);
            try {
                std::filesystem::remove(cachePath); // Loscht die Datei
                std::wcout << L"Remove: Datei im Cache geloscht: " << cachePath << std::endl;
                currentCacheSize -= size;
            } catch (const std::filesystem::filesystem_error &e) {
                std::wcout << L"Remove: Fehler beim Loschen der Datei im Cache: " << std::endl;
            }
        } else {
            std::wcout << L"Remove: Datei existiert nicht im Cache: " << cachePath << std::endl;
        }
    } else {
        // std::wcout << L"Remove: Pfad nicht im Cache gefunden: " << fullPath << std::endl;
    }
};

// private Funktioen

// Hilfsfunktion, um den Cache-Pfad zu extrahieren
std::wstring SsdCache::GetCachePathFromFullPath(const std::wstring &fullPath) {
    // Der vollstandige Pfad wird bearbeitet, um den relativen Cache-Pfad zu erhalten
    std::wstring originalPath = fullPath;
    originalPath = originalPath.substr(4); // Entferne den Laufwerksbuchstaben (z.B. "E:/")
    std::wstring originalPathForCashe = originalPath;
    originalPathForCashe.erase(std::remove(originalPathForCashe.begin(), originalPathForCashe.end(), L':'), originalPathForCashe.end());
    // Der Cache-Pfad wird erstellt
    std::wstring cachePath = cacheStammVerzeichnis + originalPathForCashe;
    std::replace(cachePath.begin(), cachePath.end(), L'\\', L'/');

    return cachePath;
}

bool SsdCache::AddFile(std::wstring &originalPath, std::wstring &cachePath, HANDLE handle, WCHAR fullPath[1284]) {
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
    std::filesystem::create_directories(std::filesystem::path(cachePath).parent_path());
    std::cout << "AddFile: Zielverzeichnisse erstellt" << std::endl;

    // Datei kopieren
    if (!CopyFileW(originalPath.c_str(), cachePath.c_str(), FALSE)) {
        std::wcout << L"AddFile: Fehler beim Kopieren in Cache: " << originalPath << std::endl;
        return false;
    }

    // Merke das Datei vorhanden
    std::wcout << L"AddFile: fullPath insert : " << fullPath << std::endl;
    cashePfade.insert(fullPath);
    return true;
};

bool SsdCache::AddFileSize(const UINT64 &fileSize) {
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

UINT64 SsdCache::Clear(const UINT64 &size) {
    UINT64 freedSize = 0;
    UINT64 removedSize = 0;
    std::wstring cacePath;
    std::vector<std::wstring> removePfade;

    for (const std::wstring fullPath : cashePfade) {
        if (removedSize < size) {
            cacePath = GetCachePathFromFullPath(fullPath);
            removedSize += SizeFromPath(cacePath);
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

UINT64 SsdCache::SizeFromPath(const std::wstring &Path) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(Path.c_str(), GetFileExInfoStandard, &fileInfo)) {
        return 0; // oder ggf. Fehlerbehandlung
    }

    ULARGE_INTEGER size;
    size.HighPart = fileInfo.nFileSizeHigh;
    size.LowPart = fileInfo.nFileSizeLow;

    return size.QuadPart;
}

bool SsdCache::ShouldCachePath(const std::wstring &fullPath) {
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
        // - Gib false zurück (→ noch nicht cachen).
        return false;
    }
}
