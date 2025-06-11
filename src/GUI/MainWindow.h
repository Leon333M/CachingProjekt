// MainWindow.h
#pragma once
#include "../ConfigLoader.h"
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>

class MainWindow : public QMainWindow {
    // Q_OBJECT
public:
    ConfigLoader *configLoader;
    QWidget window;
    QGridLayout windowLayout;
    QWidget vhddsWidget;
    QWidget cachesWidget;

public:
    MainWindow(ConfigLoader *controller);
};
