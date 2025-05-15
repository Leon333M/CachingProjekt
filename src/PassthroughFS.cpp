#include "PassthroughFS.h"
#include <iostream>

PassthroughFS::PassthroughFS(const std::string& backendPath) {
    backend = backendPath;
}

void PassthroughFS::mount(const std::string& mountPoint) {
    std::cout << "Mounting " << mountPoint << " -> " << backend << "\n";
    // TODO: Später echte FUSE-Mount-Funktion einbauen
}

void PassthroughFS::unmount() {
    std::cout << "Unmounting\n";
}