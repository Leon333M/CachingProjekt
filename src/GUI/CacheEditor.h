// CacheEditor.h
#pragma once
#include "../CacheInterface.h"
#include <QGridLayout>
#include <QLabel>
#include <QPlainTextEdit>
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
    QPlainTextEdit cashePfadeTextBlock;
    int benachrichtigerEventId;

public:
    CacheEditor(CacheInterface *cacheInterface);
    ~CacheEditor();
    void refresh();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void paintEvent(QPaintEvent *event);
};
