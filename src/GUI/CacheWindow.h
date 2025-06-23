// CacheWindow.h
#pragma once
#include "../CacheInterface.h"
#include "CacheEditor.h"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

class CacheWindow : public QWidget {
    // Q_OBJECT
private:
    CacheInterface *cache = nullptr;
    CacheEditor editor;
    QGridLayout windowLayout;
    QLabel labelCacheTyp;
    QLabel labelCurrentCacheSize;
    QLabel labelMaxCacheSize;
    QLabel labelMinZugriffsHaufigkeit;
    int benachrichtigerEventId;

public:
    CacheWindow(CacheInterface *cacheInterface);
    ~CacheWindow();
    void refresh();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    const double byteToGbyte(const UINT64 &byte) const;
    void showCacheEditor();
    void mousePressEvent(QMouseEvent *event);
};
