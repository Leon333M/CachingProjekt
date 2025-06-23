// CacheWindow.cpp
#include "CacheWindow.h"
#include "../Cache.h"
#include <QLabel>
#include <QMouseEvent>
#include <plog/Log.h>

CacheWindow::CacheWindow(CacheInterface *cacheInterface)
    : cache(cacheInterface),
      editor(cache),
      windowLayout(QGridLayout(this)) {
    int i = 0;
    windowLayout.addWidget(new QLabel(QString::fromWCharArray(cacheInterface->getCacheName().c_str())), 0, i++);
    windowLayout.addWidget(&labelCacheTyp, 0, i++);
    windowLayout.addWidget(&labelMinZugriffsHaufigkeit, 0, i++);
    windowLayout.addWidget(&labelCurrentCacheSize, 0, i++);
    windowLayout.addWidget(&labelMaxCacheSize, 0, i++);
    refresh();
    if (cache->getCacheTyp() == L"Cache") {
        Cache *cache0 = static_cast<Cache *>(cache);
        labelCurrentCacheSize.setText(QString::fromWCharArray(cache0->getRamCache().getCacheName().c_str()));
        labelMaxCacheSize.setText(QString::fromWCharArray(cache0->getSsdCache().getCacheName().c_str()));
        labelMinZugriffsHaufigkeit.hide();
    } else {
        benachrichtigerEventId = cache->registriereListener([this]() {
            QMetaObject::invokeMethod(this, [this]() { refresh(); }, Qt::QueuedConnection);
        });
    }
}

CacheWindow::~CacheWindow() {
    if (cache->getCacheTyp() == L"Cache") {
    } else {
        cache->entferneListener(benachrichtigerEventId);
    }
}

void CacheWindow::refresh() {
    labelMinZugriffsHaufigkeit.setText(QString::number(cache->getMinZugriffsHaufigkeit()));
    labelCacheTyp.setText(QString::fromWCharArray(cache->getCacheTyp().c_str()));
    labelCurrentCacheSize.setText(QString::number(byteToGbyte(cache->getCurrentCacheSize())));
    labelMaxCacheSize.setText(QString::number(byteToGbyte(cache->getMaxCacheSize())));
}

void CacheWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    refresh();
    if (cache->getCacheTyp() != L"Cache") {
        benachrichtigerEventId = cache->registriereListener([this]() {
            QMetaObject::invokeMethod(this, [this]() { refresh(); }, Qt::QueuedConnection);
        });
    }
}

void CacheWindow::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    if (cache->getCacheTyp() != L"Cache") {
        cache->entferneListener(benachrichtigerEventId);
    }
}

void CacheWindow::paintEvent(QPaintEvent *event) {}

const double CacheWindow::byteToGbyte(const UINT64 &byte) const {
    const UINT64 m = 1024;
    UINT64 kByte = byte / m;
    double mByte = kByte / m;
    double gByte = mByte / m;
    return gByte;
}

void CacheWindow::showCacheEditor() {
    editor.show();
}

void CacheWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        showCacheEditor();
    }
}
