// QtSymbolHandler.cpp
#include "QtSymbolHandler.h"
#include <QApplication>
#include <QCoreApplication>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>

QtSymbolHandler::QtSymbolHandler(GuiManager *guiManager, QIcon icon, MainWindow *mw)
    : guiManager(guiManager),
      mainWindow(mw),
      icon(icon) {

    if (icon.isNull()) {
        PLOG_DEBUG << "Icon konnte nicht geladen werden!";
    } else {
        // PLOG_DEBUG << "Icon konnte geladen werden!";
    }
    trayIcon = new QSystemTrayIcon(icon, this);
    trayIcon->setToolTip("Caching Programm");

    connect(trayIcon, &QSystemTrayIcon::activated, this, &QtSymbolHandler::handleTrayActivated);

    // Kontextmenu mit "Beenden"
    QMenu *menu = new QMenu();
    QAction *exitAction = menu->addAction("Beenden");
    connect(exitAction, &QAction::triggered, [this]() {
        this->shutdown();
    });
    QAction *showAction = menu->addAction("Gui anzeigen");
    connect(showAction, &QAction::triggered, [this]() {
        this->mainWindow->show();
    });

    trayIcon->setContextMenu(menu);

    trayIcon->show();
}

QtSymbolHandler::~QtSymbolHandler() {
    trayIcon->hide();
    delete trayIcon;
    if (exitThread.joinable()) {
        exitThread.join();
    }
}

void QtSymbolHandler::shutdown() {
    if (exitThread.joinable()) {
        exitThread.join();
    }
    exitThread = std::thread(&GuiManager ::shutdown, guiManager);
}

void QtSymbolHandler::handleTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) { // Linksklick
        QMessageBox msgBox;
        msgBox.setWindowTitle("Beenden");
        msgBox.setText("Programm wirklich beenden?");
        msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Ok) {
            this->shutdown();
        } else if (ret == QMessageBox::Open) {
            mainWindow->show();
        }
    }
}
