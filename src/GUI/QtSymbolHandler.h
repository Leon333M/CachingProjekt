// QtSymbolHandler.h
#pragma once
#include "GuiManager.h"
#include <QObject>
#include <QSystemTrayIcon>

class QtSymbolHandler : public QObject {
    // Q_OBJECT

private:
    GuiManager *guiManager;
    QIcon icon;
    QSystemTrayIcon *trayIcon;
    std::thread exitThread;

public:
    QtSymbolHandler(GuiManager *guiManager);
    ~QtSymbolHandler();
    void shutdown();

private slots:
    void handleTrayActivated(QSystemTrayIcon::ActivationReason reason);
};
