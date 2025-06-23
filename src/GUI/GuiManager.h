// GuiManager.h
#pragma once
#include "../ConfigLoader.h"
#include <thread>

class GuiManager {
private:
    std::thread qtThread;
    ConfigLoader *configLoader;

public:
    GuiManager(int argc, char *argv[], ConfigLoader *controller);
    ~GuiManager();
    void shutdown();
    void guiClose();
    void guiShow();

private:
    void startGui(int argc, char *argv[]);
};
