// ConfigLoader.cpp
#include "ConfigLoader.h"
#include "SymbolHandler.h"
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>

bool ConfigLoader::loadFromFile(const std::wstring &filename) {
    bool ret = false;
    if (loadFile(filename)) {
        verarbeiteteDaten();
    }
    return ret;
}

void ConfigLoader::start() {
    threads = std::vector<std::thread>();
    for (VirtuelleFestplatte &vhdd : vhdds) {
        threads.emplace_back(&ConfigLoader::starteVhdd, this, std::ref(vhdd));
        Sleep(1000);
    }
    for (std::thread &thread : threads) {
        thread.join();
    }
}

void ConfigLoader::clear() {
    for (const auto &cacheM : cacheMap) {
        cacheM.second->clear();
    }
    threads.clear();
    for (const auto &cacheM : cacheMap) {
        cacheM.second->clear();
    }
}

void ConfigLoader::starteVhdd(VirtuelleFestplatte &vhdd) {
    std::cout << "Starte vhdd" << std::endl;
    vhdd.start();
    std::cout << "Starte vhdd zuende" << std::endl;
}

bool ConfigLoader::loadFile(const std::wstring &filename) {

    std::wifstream file(filename);
    // Stelle sicher, dass die Datei geöffnet werden konnte
    if (!file.is_open()) {
        std::wcerr << L"Fehler beim Öffnen der Datei: " << filename << std::endl;
        return false;
    }

    // Lese die Datei zeilenweise
    std::wstring line;
    while (std::getline(file, line)) {
        // Konvertiere die wstring-Zeile in einen string
        std::string strLine = wstringToString(line);
        configData.push_back(strLine);
    }
    file.close();

    return true;
}

void ConfigLoader::verarbeiteteDaten() {
    for (std::string zeile : configData) {
        std::stringstream ss = std::stringstream(zeile);
        std::string erstesWort;
        ss >> erstesWort;
        char firstChar = erstesWort[0];

        switch (firstChar) {
            case 'S':
                if (erstesWort == "SsdCache") {
                    erstelleSsdCache(zeile);
                    break;
                }
            case 'R':
                if (erstesWort == "RamCache") {
                    erstelleRamCache(zeile);
                    break;
                }
            case 'C':
                if (erstesWort == "Cache") {
                    erstelleCache(zeile);
                    break;
                }
            case 'V':
                if (erstesWort == "Vhdd") {
                    erstelleVhdd(zeile);
                    break;
                }
            case 'L':
                if (erstesWort == "Log") {
                    erstelleLog(zeile);
                    break;
                }
            case 'H':
                if (erstesWort == "HideTerminal") {
                    erstelleHideTerminal(zeile);
                    break;
                }
            default:
                std::cerr << "Unbekannter Typ: " << zeile << std::endl;
                break;
        }
    }
}

void ConfigLoader::erstelleSsdCache(std::string zeile) {
    std::stringstream ss(zeile);
    std::string cacheTyp, cacheName, cacheValueName;
    int cacheSize, minZugriffsHaufigkeit;
    if (ss >> cacheTyp >> cacheName >> cacheValueName >> cacheSize) {
        if (ss >> minZugriffsHaufigkeit) {
            // 2. Zahl existirt und ist geladen
            cacheMap.emplace(cacheName, std::make_shared<SsdCache>(SsdCache(stringToWString(cacheName), stringToWString(cacheValueName), cacheSize, minZugriffsHaufigkeit)));
        } else {
            cacheMap.emplace(cacheName, std::make_shared<SsdCache>(SsdCache(stringToWString(cacheName), stringToWString(cacheValueName), cacheSize)));
        }
    } else {
        std::cerr << "Unbekannter Zeilen inhalt: " << zeile << std::endl;
    }
}

void ConfigLoader::erstelleRamCache(std::string zeile) {
    std::stringstream ss(zeile);
    std::string cacheTyp, cacheName;
    int cacheSize, minZugriffsHaufigkeit;
    if (ss >> cacheTyp >> cacheName >> cacheSize) {
        if (ss >> minZugriffsHaufigkeit) {
            // 2. Zahl existirt und ist geladen
            cacheMap.emplace(cacheName, std::make_shared<RamCache>(RamCache(stringToWString(cacheName), cacheSize, minZugriffsHaufigkeit)));
        } else {
            cacheMap.emplace(cacheName, std::make_shared<RamCache>(RamCache(stringToWString(cacheName), cacheSize)));
        }
    } else {
        std::cerr << "Unbekannter Zeilen inhalt: " << zeile << std::endl;
    }
}

void ConfigLoader::erstelleCache(std::string zeile) {
    std::stringstream ss(zeile);
    std::string cacheTyp, cacheName, cacheName1, cacheName2;
    if (ss >> cacheTyp >> cacheName >> cacheName1 >> cacheName2) {
        CacheInterface &ramCache = *cacheMap[cacheName1];
        CacheInterface &ssdCache = *cacheMap[cacheName2];
        cacheMap.emplace(cacheName, std::make_shared<Cache>(Cache(stringToWString(cacheName), ramCache, ssdCache)));
    } else {
        std::cerr << "Unbekannter Zeilen inhalt: " << zeile << std::endl;
    }
}

void ConfigLoader::erstelleVhdd(std::string zeile) {
    std::stringstream ss(zeile);
    std::string cacheTyp, hddValueName, vhddValueName, cacheName;
    if (ss >> cacheTyp >> hddValueName >> vhddValueName >> cacheName) {
        CacheInterface &cache = *cacheMap[cacheName];
        vhdds.emplace_back(VirtuelleFestplatte(stringToWString(hddValueName), stringToWString(vhddValueName), cache));
    } else {
        std::cerr << "Unbekannter Zeilen inhalt: " << zeile << std::endl;
    }
}

void ConfigLoader::erstelleLog(std::string zeile) {
    std::stringstream ss(zeile);
    int logLevel;
    std::string cacheTyp, logDatei;
    if (ss >> cacheTyp >> logLevel) {
        this->logLevel = logLevel;
        if (ss >> logDatei) {
            this->logDatei = logDatei;
        }
    } else {
        std::cerr << "Unbekannter Zeilen inhalt: " << zeile << std::endl;
    }
}

void ConfigLoader::erstelleHideTerminal(std::string zeile) {
    std::stringstream ss(zeile);
    std::string typ, hideString;
    if (ss >> typ >> hideString) {
        if (hideString == "true") {
            // Hole das Konsolenfenster des aktuellen Prozesses
            HWND hwnd = GetConsoleWindow();
            // Verberge das Konsolenfenster
            ShowWindow(hwnd, SW_HIDE);
            // init exit symbol
            symbolHandler = std::make_unique<SymbolHandler>(this);
        } else if (hideString == "false") {
            // Verberge das Konsolenfenster nicht
        } else {
            std::cerr << "Unbekanntes Wort " << hideString << " in Zeile " << zeile << std::endl;
        }
    } else {
        std::cerr << "Unbekannter Zeilen inhalt: " << zeile << std::endl;
    }
}

int ConfigLoader::getLogLevel() {
    return logLevel;
}
std::string ConfigLoader::getLogDatei() {
    return logDatei;
}

std::wstring ConfigLoader::stringToWString(const std::string &str) {
    return std::wstring(str.begin(), str.end());
}

std::string ConfigLoader::wstringToString(const std::wstring &wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string str = converter.to_bytes(wstr);
    return str;
}
