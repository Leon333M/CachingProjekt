// QtSymbolHandler.h
#pragma once
#include "../ConfigLoader.h"
#include <QObject>
#include <QSystemTrayIcon>

class QtSymbolHandler : public QObject {
    // Q_OBJECT

private:
    QSystemTrayIcon *trayIcon;
    ConfigLoader *configLoader;
    QIcon icon;

public:
    QtSymbolHandler(ConfigLoader *configLoader);
    ~QtSymbolHandler();

private slots:
    void handleTrayActivated(QSystemTrayIcon::ActivationReason reason);
};
