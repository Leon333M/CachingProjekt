// main.cpp
#include "VirtuelleFestplatte.h"
#include <iostream>
#include <windows.h>

int main(int argc, char *argv[]) {
    try {
        std::cout << "Init virtuelles Laufwerk G:, das F: spiegelt" << std::endl;
        std::wstring orginalVolume = L"F:";
        std::wstring neuesVolume = L"G:";
        std::wstring cacheVolume = L"E:";
        std::cout << "Argumente : " << argc << " " << argv << std::endl;
        if (argc == 4) {
            orginalVolume = std::wstring(argv[1], argv[1] + strlen(argv[1]));
            neuesVolume = std::wstring(argv[2], argv[2] + strlen(argv[2]));
            cacheVolume = std::wstring(argv[3], argv[3] + strlen(argv[3]));
        } else {
            for (int i = 0; i < argc; i++) {
                std::cout << argv[i] << std::endl;
            }
            MessageBox(NULL, "Fehler falsche Argumente.", "Fehler", MB_OK);
        }

        SsdCache ssdCache = SsdCache(cacheVolume, 8);
        RamCache ramCache = RamCache(8);
        Cache cacheBackend = Cache(ssdCache, ramCache);
        VirtuelleFestplatte vhdd = VirtuelleFestplatte(orginalVolume, neuesVolume, ramCache);

        std::string text = " virtuelles Laufwerk G:, das F: spiegelt";
        std::cout << "Starte" << text << std::endl;
        vhdd.start();

        std::cout << "Stoppe" << text << std::endl;
        vhdd.stop();

        std::cout << "ende main" << std::endl;
        return 0;

    } catch (...) {
        std::cout << "Error das progam ist abgestuertzt\n";
        return 0;
    }
}
