// QtSymbolHandler.cpp
#include "QtSymbolHandler.h"
#include <QApplication>
#include <QCoreApplication>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>

QtSymbolHandler::QtSymbolHandler(ConfigLoader *configLoader) : configLoader(configLoader) {
    icon = QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon);

    if (icon.isNull()) {
        PLOG_DEBUG << "Icon konnte nicht geladen werden!";
    } else {
        PLOG_DEBUG << "Icon konnte geladen werden!";
    }
    trayIcon = new QSystemTrayIcon(icon, this);
    trayIcon->setToolTip("Caching Programm");

    connect(trayIcon, &QSystemTrayIcon::activated, this, &QtSymbolHandler::handleTrayActivated);

    // Optional: Kontextmenü mit "Beenden"
    QMenu *menu = new QMenu();
    QAction *exitAction = menu->addAction("Beenden");
    connect(exitAction, &QAction::triggered, [this]() {
        this->configLoader->clear();
    });
    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

QtSymbolHandler::~QtSymbolHandler() {
    trayIcon->hide();
    delete trayIcon;
}

void QtSymbolHandler::handleTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) { // Linksklick
        QMessageBox::information(nullptr, "Beenden", "Programm wird beendet...");
        configLoader->clear();
    }
}
