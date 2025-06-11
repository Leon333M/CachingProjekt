// CacheWindow.h
#pragma once
#include "../CacheInterface.h"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

class CacheWindow : public QWidget {
    // Q_OBJECT
private:
    CacheInterface *cache = nullptr;
    std::string cacheName;
    QGridLayout windowLayout;
    QLabel labelCacheTyp;
    QLabel labelCurrentCacheSize;
    QLabel labelMaxCacheSize;

public:
    CacheWindow(std::string name, CacheInterface *cacheInterface);
    void refresh();
    void paintEvent(QPaintEvent *event);
};
