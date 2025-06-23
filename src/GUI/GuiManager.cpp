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
    qApp->closeAllWindows();
    qApp->quit();
    Sleep(100); // Gebe Qt Zeit zum beenden
}

void GuiManager::startGui(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ShutdownEventFilter *filter = new ShutdownEventFilter(this);
    app.installNativeEventFilter(filter);
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon);
    QtSymbolHandler symbol(this, icon);
    MainWindow mw = MainWindow(configLoader, icon);
    PLOG_DEBUG << "GUI beendet " << app.exec();
}
