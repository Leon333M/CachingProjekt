// ConfigLoader.h
#pragma once
#include "VirtuelleFestplatte.h"
#include <memory>
#include <thread>

class ConfigLoader {
private:
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> cacheMap;
    std::vector<VirtuelleFestplatte> vhdds;
    std::vector<std::string> configData;
    std::vector<std::thread> threads;
    std::string logDatei = "";
    int logLevel = 1;
    bool hideGui = true;
    bool isStarted = false;

public:
    ~ConfigLoader();
    bool loadFromFile(const std::wstring &filename);
    void start();
    void clear();
    void shutdown();
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
     *
     * @return int LogLevel (1 = FATAL, 6 = VERBOSE)
     */
    int getLogLevel();
    std::string getLogDatei();
    std::vector<std::string> getConfigData() { return configData; };
    std::vector<VirtuelleFestplatte> *getVhdds() { return &vhdds; };
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> *getCacheMap() { return &cacheMap; };
    bool getHideGui() { return hideGui; };
    bool getIsStarted() { return isStarted; };
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
    void erstelleLog(std::string zeile);
    void erstelleHideTerminal(std::string zeile);
    void erstelleHideGui(std::string zeile);
};
