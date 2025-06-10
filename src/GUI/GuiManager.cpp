// GuiManager.cpp
#include "GuiManager.h"
#include "MainWindow.h"

GuiManager::GuiManager(int argc, char *argv[], ConfigLoader *controller)
    : configLoader(controller) {
    startGui(argc, argv);
}

void GuiManager::startGui(int argc, char *argv[]) {
    QApplication app(argc, argv);
    new MainWindow(configLoader);
    app.exec();
}
