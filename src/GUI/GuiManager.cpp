// GuiManager.cpp
#include "GuiManager.h"
#include "MainWindow.h"
#include "QtSymbolHandler.h"
#include "ShutdownEventFilter.h"
#include <QStyle>

GuiManager::GuiManager(int argc, char *argv[], ConfigLoader *controller)
    : configLoader(controller) {
    qtThread = std::thread(&GuiManager::startGui, this, argc, argv);
}

GuiManager::~GuiManager() {
    shutdown();
    if (exitThread.joinable()) {
        exitThread.join();
    }
    if (qtThread.joinable()) {
        qtThread.join();
    }
}

void GuiManager::shutdown() {
    if (exitThread.joinable()) {
        PLOG_DEBUG << "shutdown schon gestartet";
    } else {
        exitThread = std::thread(&GuiManager ::shutdownThreadFunktion, this);
    }
}

void GuiManager::shutdownThreadFunktion() {
    if (!configLoader->getIsStarted()) {
        PLOG_DEBUG << "warte auf ConfigLoader";
        while (!configLoader->getIsStarted()) {
            Sleep(100);
        }
    }
    PLOG_DEBUG << "shutdown guiClose";
    guiClose();
    PLOG_DEBUG << "shutdown configLoader";
    configLoader->shutdown();
    PLOG_DEBUG << "shutdown ende";
}

void GuiManager::guiClose() {
    PLOG_DEBUG << "schliesse alle Fenster";
    qApp->closeAllWindows();
    PLOG_DEBUG << "beende GUI";
    qApp->quit();
    if (qtThread.joinable()) {
        PLOG_DEBUG << "warte auf GUI";
        qtThread.join();
    }
}

void GuiManager::startGui(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ShutdownEventFilter *filter = new ShutdownEventFilter(this);
    app.installNativeEventFilter(filter);
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon);
    MainWindow mw = MainWindow(configLoader, icon);
    QtSymbolHandler symbol(this, icon, &mw);
    PLOG_DEBUG << "GUI beendet " << app.exec();
}
