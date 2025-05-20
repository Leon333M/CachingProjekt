// passthrough_backend.cpp
#include "passthrough_backend.h"
// #include "passthrough.c"

#include <string>
#include <codecvt>
#include <locale>
#include <windows.h>
#include <iostream>

#include <winfsp/winfsp.h>
#include <strsafe.h>
#include <thread>

#define PROGNAME "passthrough"
extern "C" {
//FspServiceRun;
//NT_SUCCESS;
//FspLoad;
static NTSTATUS SvcStart(FSP_SERVICE *Service, ULONG argc, PWSTR *argv);
static NTSTATUS SvcStop(FSP_SERVICE *Service);
};
std::thread bekendThrad;

void BekendThradFunktion() {
    std::cout << " BekendThradFunktion " << std::endl;
    FspServiceRun(L"" PROGNAME, SvcStart, SvcStop, 0);
}

void starteBekendThrad() {
    std::cout << " starteBekendThrad " << std::endl;
    // bekendThrad = std::thread(BekendThradFunktion);
    BekendThradFunktion();
}

void stopeBekendThrad() {
    std::cout << " starteBekendThrad " << std::endl;
    //bekendThrad.stop();
    std::cout << " nicht noetig da in start schon bendet " << std::endl;
}


bool StartPassthroughBackend(const std::string& backendPath, const std::string& mountPoint) {
    std::cout << " StartPassthroughBackend " << std::endl;
    if (!NT_SUCCESS(FspLoad(0))) {
        std::cout << "Fehler beim laden von FspLoad" << std::endl;
        return false;
    }
    starteBekendThrad();
    return true;
}

void StopPassthroughBackend() {
    std::cout << "Passthrough wird bendet." << std::endl;
    stopeBekendThrad();
    std::cout << "Passthrough erfolgreich bendet." << std::endl;
}