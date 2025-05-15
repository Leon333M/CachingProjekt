#include <iostream>
#include "PassthroughFS.h"

int main() {
    std::cout << "Starte virtuelles Laufwerk G:, das F: spiegelt\n";

    PassthroughFS fs("F:\\");  // Backend-Quelle
    fs.mount("G:");            // Mountpoint

    std::cout << "Dateisystem lauuft. Druecke Enter zum Beenden.\n";
    std::cin.get();

    fs.unmount();
    return 0;
}