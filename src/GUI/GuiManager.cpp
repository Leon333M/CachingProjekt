// GuiManager.cpp
#include "GuiManager.h"
#include "MainWindow.h"
#include "QtSymbolHandler.h"
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

void GuiManager::shutdown() {
    PLOG_DEBUG << "shutdown guiClose";
    guiClose();
    PLOG_DEBUG << "shutdown configLoader";
    configLoader->shutdown();
    PLOG_DEBUG << "shutdown ende";
}

void GuiManager::guiClose() {
    qApp->quit();
    Sleep(100); // Gebe Qt Zeit zum beenden
}

void GuiManager::startGui(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ShutdownEventFilter *filter = new ShutdownEventFilter(this);
    app.installNativeEventFilter(filter);
    QtSymbolHandler symbol(this);
    MainWindow mw = MainWindow(configLoader);
    PLOG_DEBUG << "GUI beendet " << app.exec();
}
