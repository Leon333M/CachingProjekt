// MainWindow.h
#pragma once
#include "../ConfigLoader.h"
#include "CacheWindow.h"
#include "LinienOverlay.h"
#include "VhddWindow.h"
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>

struct CachePair {
    CacheInterface *cache = nullptr;
    CacheWindow *cacheWidget = nullptr;
    bool isAdded = false;
    CachePair(CacheInterface *c, CacheWindow *w) : cache(c), cacheWidget(w) {}
};

struct VhddPair {
    VirtuelleFestplatte *vhdd = nullptr;
    VhddWindow *vhddWidget = nullptr;
    VhddPair(VirtuelleFestplatte *v, VhddWindow *w) : vhdd(v), vhddWidget(w) {}
};

class AutoRowGridLayout : public QGridLayout {
    std::map<int, int> currentRowPerColumn;

public:
    AutoRowGridLayout(QWidget *parent = nullptr)
        : QGridLayout(parent) {}

    void addWidgetAutoRow(QWidget *widget, int column = 0) {
        int row = currentRowPerColumn[column]++;
        addWidget(widget, row, column);
    }
};

class MainWindow : public QMainWindow {
    // Q_OBJECT
public:
    ConfigLoader *configLoader;
    QWidget window;
    AutoRowGridLayout windowLayout;
    LinienOverlay overlay;

public:
    MainWindow(ConfigLoader *controller);
    void resizeEvent(QResizeEvent *event);

private:
    CachePair &findeCachePairMitCacheName(const std::wstring &cacheName, std::vector<CachePair> &vhddPairs);
    void baueCacheRekursivAuf(CachePair &cp, int spalte, std::vector<CachePair> &alleCachePairs);
};
