// CacheWindow.cpp
#include "CacheWindow.h"
#include "../Cache.h"
#include <QLabel>
#include <plog/Log.h>

class AutoRowGridLayout : public QGridLayout {
    std::map<int, int> currentRowPerColumn;

public:
    void addWidgetAutoRow(QWidget *widget, int column = 0) {
        int row = currentRowPerColumn[column]++;
        addWidget(widget, row, column);
    }
};

CacheWindow::CacheWindow(CacheInterface *cacheInterface)
    : windowLayout(QGridLayout(this)) {
    cache = cacheInterface;
    int i = 0;
    windowLayout.addWidget(new QLabel(QString::fromWCharArray(cacheInterface->getCacheName().c_str())), 0, i++);
    windowLayout.addWidget(&labelCacheTyp, 0, i++);
    windowLayout.addWidget(&labelCurrentCacheSize, 0, i++);
    windowLayout.addWidget(&labelMaxCacheSize, 0, i++);
    windowLayout.addWidget(&labelMaxCacheSize, 0, i++);
    refresh();
    if (cache->getCacheTyp() == L"Cache") {
        Cache *cache0 = static_cast<Cache *>(cache);
        labelCurrentCacheSize.setText(QString::fromWCharArray(cache0->getRamCache().getCacheName().c_str()));
        labelMaxCacheSize.setText(QString::fromWCharArray(cache0->getSsdCache().getCacheName().c_str()));
    } else {
        cache->registriereListener([this]() {
            QMetaObject::invokeMethod(this, [this]() { refresh(); }, Qt::QueuedConnection);
        });
    }
}

void CacheWindow::refresh() {
    labelCacheTyp.setText(QString::fromWCharArray(cache->getCacheTyp().c_str()));
    labelCurrentCacheSize.setText(QString::number(byteToGbyte(cache->getCurrentCacheSize())));
    labelMaxCacheSize.setText(QString::number(byteToGbyte(cache->getMaxCacheSize())));
}

void CacheWindow::paintEvent(QPaintEvent *event) {}

const double CacheWindow::byteToGbyte(const UINT64 &byte) const {
    const UINT64 m = 1024;
    UINT64 kByte = byte / m;
    double mByte = kByte / m;
    double gByte = mByte / m;
    return gByte;
}
