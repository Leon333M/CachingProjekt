// main.cpp
#include "PassthroughFS.h"
#include <iostream>
#include <windows.h> 

int main() {
    try {
        std::cout << "Mesage Box Anzeigen\n";
        std::cout << "Starte virtuelles Laufwerk G:, das F: spiegelt\n";

        PassthroughFS fs("F:\\");  // Backend-Quelle
        fs.mount("G:");            // Mountpoint

        if (!fs.isMounted()) {
            std::cerr << "main: Mount fehlgeschlagen. Programm wird beendet.\n";
            return 1;
        }

        std::cout << "Dateisystem lauuft. Druecke Enter zum Beenden.\n";
        std::cin.get();

        fs.unmount();
        return 0;
    } catch (...) {
        std::cout << "Error das progam ist abgestuertzt\n";
    }
}