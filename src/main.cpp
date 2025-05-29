// main.cpp
#include "VirtuelleFestplatte.h"
#include <iostream>
#include <windows.h>

int main(int argc, wchar_t **argv) {
    try {
        std::cout << "Init virtuelles Laufwerk G:, das F: spiegelt" << std::endl;
        std::wstring orginalVolume = L"F:";
        std::wstring neuesVolume = L"G:";
        std::wstring cacheVolume = L"E:";
        Cache cacheBackend = Cache(cacheVolume);
        VirtuelleFestplatte vhdd = VirtuelleFestplatte(orginalVolume, neuesVolume, cacheBackend);

        std::cout << "Starte virtuelles Laufwerk G:, das F: spiegelt" << std::endl;
        vhdd.start();

        std::cout << "Stoppe virtuelles Laufwerk G:, das F: spiegelt" << std::endl;
        vhdd.stop();

        std::cout << "ende main" << std::endl;
        return 0;

    } catch (...) {
        std::cout << "Error das progam ist abgestuertzt\n";
        return 0;
    }
}
