// GuiManager.cpp
#include "GuiManager.h"
#include "MainWindow.h"

GuiManager::GuiManager(int argc, char *argv[], ConfigLoader *controller)
    : configLoader(controller) {
    qtThread = std::thread(&GuiManager::startGui, this, argc, argv);
}

GuiManager::~GuiManager() {
    if (qtThread.joinable()) {
        qtThread.join();
    }
}

void GuiManager::startGui(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow mw = MainWindow(configLoader);
    app.exec();
}
