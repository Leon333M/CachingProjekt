// SsdCache.cpp
#include "SsdCache.h"
#include <filesystem>
#include <iostream>
#include <strsafe.h>
#include <winfsp/winfsp.h>

#define FULLPATH_SIZE (MAX_PATH + FSP_FSCTL_TRANSACT_PATH_SIZEMAX / sizeof(WCHAR))

bool SsdCache::Read(HANDLE handle, LPVOID buffer, DWORD length, LPDWORD bytesTransferred, LPOVERLAPPED overlapped) {
    WCHAR FullPath[FULLPATH_SIZE];
    GetFinalPathNameByHandleW(handle, FullPath, FULLPATH_SIZE - 1, 0);
    // std::wcout << L"ReadCash : " << FullPath << std::endl;
    std::wstring originalPath = FullPath;
    originalPath = originalPath.substr(4);
    std::wstring cachePath = GetCachePathFromFullPath(FullPath);

    if (!(cashePfade.find(FullPath) != cashePfade.end())) {
        // add Datei zum ReadCash
        std::wcout << L"ReadCash: FullPath NICHT vorhanden, kopiere: " << originalPath << L" zu : " << cachePath << std::endl;
        // Sicherstellen, dass die Zielverzeichnisse existieren
        std::filesystem::create_directories(std::filesystem::path(cachePath).parent_path());
        std::cout << "ReadCash: Zielverzeichnisse existieren" << std::endl;
        // Datei kopieren
        if (!CopyFileW(originalPath.c_str(), cachePath.c_str(), FALSE)) {
            std::wcout << L"Fehler beim Kopieren in Cache: " << originalPath << std::endl;
            return false;
        }
        std::wcout << L"ReadCash: FullPath insert : " << FullPath << std::endl;
        cashePfade.insert(FullPath);
    } else {
        // FullPath ist bereits in cashePfade
        std::wcout << L"ReadCash: FullPath vorhanden : " << FullPath << std::endl;
    }
    // von ReadCash lesen
    // Jetzt von E:/Cashe/... lesen
    HANDLE cacheHandle = CreateFileW(cachePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (cacheHandle == INVALID_HANDLE_VALUE) {
        std::wcout << L"Fehler beim Offnen der Cache-Datei: " << cachePath << std::endl;
        return false;
    }
    BOOL result = ReadFile(cacheHandle, buffer, length, bytesTransferred, overlapped);
    CloseHandle(cacheHandle);
    if (!result) {
        std::wcout << L"ReadCash: Fehler beim laden von Cashe : " << cachePath << std::endl;
        return false;
    }
    std::wcout << L"ReadCash: FullPath vorhanden und von Cashe geladen : " << cachePath << std::endl;
    // result = false; // erstmal, entferen ich spater
    return result;
};

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
            try {
                std::filesystem::remove(cachePath); // Löscht die Datei
                std::wcout << L"Datei im Cache gelöscht: " << cachePath << std::endl;
            } catch (const std::filesystem::filesystem_error &e) {
                std::wcout << L"Fehler beim Löschen der Datei im Cache: " << std::endl;
            }
        } else {
            std::wcout << L"Datei existiert nicht im Cache: " << cachePath << std::endl;
        }
        std::wcout << L"RemoveFromCache: Entfernt aus Cache: " << fullPath << std::endl;
    } else {
        std::wcout << L"RemoveFromCache: Pfad nicht im Cache gefunden: " << fullPath << std::endl;
    }
};

void SsdCache::Clear() {
    std::cout << "ClearCacheDirectory" << std::endl;
    const std::filesystem::path cacheDir = L"E:/Cashe";
    std::error_code ec; // Fur Fehlerbehandlung ohne Exceptions
    if (!std::filesystem::exists(cacheDir, ec)) {
        std::wcout << L"Cache-Verzeichnis existiert nicht: " << cacheDir.wstring() << std::endl;
        return;
    }
    for (const auto &entry : std::filesystem::directory_iterator(cacheDir, ec)) {
        if (ec) {
            std::wcerr << L"Fehler beim Iterieren: " << ec.message().c_str() << std::endl;
            return;
        }
        std::filesystem::remove_all(entry.path(), ec);
        if (ec) {
            std::wcerr << L"Fehler beim Loschen von " << entry.path().wstring() << L": " << ec.message().c_str() << std::endl;
        } else {
            std::wcout << L"Geloscht: " << entry.path().wstring() << std::endl;
        }
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
    std::wstring cachePath = L"E:/Cashe/" + originalPathForCashe;
    std::replace(cachePath.begin(), cachePath.end(), L'\\', L'/');

    return cachePath;
}
