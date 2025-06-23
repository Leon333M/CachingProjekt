// CacheEditor.h
#pragma once
#include "../CacheInterface.h"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

class CacheEditor : public QWidget {
    // Q_OBJECT
private:
    CacheInterface *cache = nullptr;
    QGridLayout windowLayout;
    QLabel labelCacheTyp;
    QLabel labelCurrentCacheSize;
    QLabel labelMaxCacheSize;
    QLabel labelMinZugriffsHaufigkeit;
    int benachrichtigerEventId;

public:
    CacheEditor(CacheInterface *cacheInterface);
    ~CacheEditor();
    void refresh();
    void showEditor();
    void hideEditor();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
};
