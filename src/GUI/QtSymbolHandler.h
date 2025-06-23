// QtSymbolHandler.h
#pragma once
#include "GuiManager.h"
#include "MainWindow.h"
#include <QObject>
#include <QSystemTrayIcon>

class QtSymbolHandler : public QObject {
    // Q_OBJECT

private:
    GuiManager *guiManager;
    MainWindow *mainWindow;
    QIcon icon;
    QSystemTrayIcon *trayIcon;
    std::thread exitThread;

public:
    QtSymbolHandler(GuiManager *guiManager, QIcon icon, MainWindow *mw);
    ~QtSymbolHandler();
    void shutdown();

private slots:
    void handleTrayActivated(QSystemTrayIcon::ActivationReason reason);
};
