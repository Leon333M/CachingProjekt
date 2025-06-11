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
}

void CacheWindow::refresh() {
    PLOG_VERBOSE << "refresh " << cacheName;
    labelCacheTyp.setText(QString::fromUtf8(cache->getCacheTyp()));
    labelCurrentCacheSize.setText(QString::number(cache->getCurrentCacheSize()));
    labelMaxCacheSize.setText(QString::number(cache->getMaxCacheSize()));
}

void CacheWindow::paintEvent(QPaintEvent *event) {
    bool oldSignal = this->blockSignals(true);
    refresh();
    this->blockSignals(oldSignal);
}
