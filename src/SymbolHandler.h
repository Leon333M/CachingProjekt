// SymbolHandler.h
#pragma once
#include "ConfigLoader.h"
#include <shellapi.h>
#include <thread>

class SymbolHandler {
private:
    std::thread thread;

public:
    SymbolHandler(ConfigLoader *configLoader);

private:
    void startSymbolHandler();
};
