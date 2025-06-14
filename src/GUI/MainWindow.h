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
    CachePair(CacheInterface *c, CacheWindow *w) : cache(c), cacheWidget(w) {}
};

struct VhddPair {
    VirtuelleFestplatte *vhdd = nullptr;
    VhddWindow *vhddWidget = nullptr;
    VhddPair(VirtuelleFestplatte *v, VhddWindow *w) : vhdd(v), vhddWidget(w) {}
};

class MainWindow : public QMainWindow {
    // Q_OBJECT
public:
    ConfigLoader *configLoader;
    QWidget window;
    QGridLayout windowLayout;
    QWidget vhddsWidget;
    QWidget cachesWidget;
    LinienOverlay overlay;

public:
    MainWindow(ConfigLoader *controller);
    void resizeEvent(QResizeEvent *event);

private:
    CachePair findeCachePairMitCacheName(const std::wstring &cacheName, std::vector<CachePair> &vhddPairs);
    void baueCacheRekursivAuf(CachePair &cp, int &zeile, int spalte, std::vector<CachePair> &alleCachePairs);
};
