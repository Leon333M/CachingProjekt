// PassthroughFS.cpp
#include "PassthroughFS.h"
#include "passthrough_backend.h"
#include <iostream>

PassthroughFS::PassthroughFS(const std::string backendPath){
    mounted = false;
    backend = backendPath;
    wBackend.assign(backend.begin(), backend.end());
}

void PassthroughFS::mount(const std::string mountPointPath) {
    mountPoint = mountPointPath;
    wMountPoint.assign(mountPoint.begin(), mountPoint.end());
    std::cout << "Mounting " << mountPoint << " -> " << backend << "\n";

    if (!StartPassthroughBackend(wBackend.c_str(), wMountPoint.c_str())) {
        std::cerr << "Mount fehlgeschlagen!\n";
        return;
    }
    mounted = true;

    std::cout << "Virtuelles Dateisystem läuft. Drücke Enter zum Beenden.\n";
}

void PassthroughFS::unmount() {
    std::cout << "Unmounting\n";
    StopPassthroughBackend();
    mounted = false;
    std::cout << "Unmounting fertig\n";
}
