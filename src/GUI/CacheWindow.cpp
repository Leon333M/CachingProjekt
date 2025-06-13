// CacheWindow.cpp
#include "CacheWindow.h"
#include <QLabel>
#include <plog/Log.h>

CacheWindow::CacheWindow(std::string name, CacheInterface *cacheInterface)
    : windowLayout(QGridLayout(this)) {
    cacheName = name;
    cache = cacheInterface;
    int i = 0;
    windowLayout.addWidget(new QLabel(QString::fromUtf8(name)), 0, i++);
    windowLayout.addWidget(&labelCacheTyp, 0, i++);
    windowLayout.addWidget(&labelCurrentCacheSize, 0, i++);
    windowLayout.addWidget(&labelMaxCacheSize, 0, i++);
    refresh();
    cache->registriereListener([this]() {
        QMetaObject::invokeMethod(this, [this]() { refresh(); }, Qt::QueuedConnection);
    });
}

void CacheWindow::refresh() {
    PLOG_DEBUG << cacheName;
    labelCacheTyp.setText(QString::fromUtf8(cache->getCacheTyp()));
    labelCurrentCacheSize.setText(QString::number(byteToGbyte(cache->getCurrentCacheSize())));
    labelMaxCacheSize.setText(QString::number(byteToGbyte(cache->getMaxCacheSize())));
}

void CacheWindow::paintEvent(QPaintEvent *event) {
    bool oldSignal = this->blockSignals(true);
    refresh();
    this->blockSignals(oldSignal);
}

const double CacheWindow::byteToGbyte(const UINT64 &byte) const {
    const UINT64 m = 1024;
    UINT64 kByte = byte / m;
    double mByte = kByte / m;
    double gByte = mByte / m;
    return gByte;
}
