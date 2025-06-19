// GuiManager.cpp
#include "GuiManager.h"
#include "MainWindow.h"
#include "ShutdownEventFilter.h"

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
    ShutdownEventFilter *filter = new ShutdownEventFilter();
    app.installNativeEventFilter(filter);
    MainWindow mw = MainWindow(configLoader);
    app.exec();
}
