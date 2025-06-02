// main.cpp
#include "ConfigLoader.h"
#include "LogManager.h"
#include "SymbolHandler.h"
#include <filesystem>
#include <iostream>
#include <windows.h>

int main(int argc, char *argv[]) {
    try {
        // Init Argumente
        std::cout << "Init Argumente" << std::endl;
        std::cout << "Argumente : " << argc << " " << argv << std::endl;
        std::string file = "";
        if (argc == 2) {
            file = argv[1];
        } else if (argc == 1) {
            file = argv[0];
            std::filesystem::path path(file);
            std::string dir = path.parent_path().string();
            std::string newFile = dir + "\\config.txt";
            file = newFile;
        } else {
            for (int i = 0; i < argc; i++) {
                std::cout << argv[i] << std::endl;
            }
            MessageBox(NULL, "Fehler falsche Argumente.", "Fehler", MB_OK);
            return 0;
        }
        if (!std::filesystem::exists(file)) {
            MessageBox(NULL, "Fehler config nicht gefuden", "Fehler", MB_OK);
            return 0;
        }

        // init Config
        ConfigLoader configLoader;
        configLoader.loadFromFile(configLoader.stringToWString(file));
        int loglevel = configLoader.getLogLevel();
        loglevel = 6;

        // init plog
        LogManager logManager(loglevel);

        // Verberge das Konsolenfenster
        if (loglevel == 0) {
            // Hole das Konsolenfenster des aktuellen Prozesses
            HWND hwnd = GetConsoleWindow();
            // Verberge das Konsolenfenster
            ShowWindow(hwnd, SW_HIDE);
            // init exit symbol
            SymbolHandler symbolHandler = SymbolHandler(&configLoader);
        }

        // Starte virtuelles Laufwerk
        PLOG_DEBUG << "starte virtuelles Laufwerk:, das Hdd spiegelt.";
        configLoader.start();
        PLOG_DEBUG << "ende main";
        return 0;

    } catch (...) {
        std::cout << "Error das progam ist abgestuertzt" << std::endl;
        return 0;
    }
}
