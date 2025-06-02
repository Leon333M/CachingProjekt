// ConfigLoader.h
#pragma once
#include "VirtuelleFestplatte.h"
#include <thread>

class ConfigLoader {
private:
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> cacheMap;
    std::vector<VirtuelleFestplatte> vhdds;
    std::vector<std::string> configData;
    std::vector<std::thread> threads;
    int LogLevel = 0;

public:
    bool loadFromFile(const std::wstring &filename);
    void start();
    void clear();
    /**
     * @brief Gibt das geladene Log-Level zuruck.
     *
     * Log-Level Ubersicht:
     *
     * 6 VERBOSE – sehr detaillierte Debug-Informationen (selten verwendet)
     * 5 DEBUG   – Debug-Ausgaben für Entwickler
     * 4 INFO    – Standard-Informationen zur Laufzeit
     * 3 WARNING – Warnungen, Programm lauft aber weiter
     * 2 ERROR   – Fehler, der eine Funktion verhindert hat
     * 1 FATAL   – Schwerwiegender Fehler, Programm bricht ggf. ab
     * 0 None
     *
     * @return int LogLevel (0 = None, 6 = VERBOSE)
     */
    int getLogLevel();
    std::wstring stringToWString(const std::string &str);
    std::string wstringToString(const std::wstring &wstr);

private:
    void starteVhdd(VirtuelleFestplatte &vhdd);
    bool loadFile(const std::wstring &filename);
    void verarbeiteteDaten();
    void erstelleSsdCache(std::string zeile);
    void erstelleRamCache(std::string zeile);
    void erstelleCache(std::string zeile);
    void erstelleVhdd(std::string zeile);
};
