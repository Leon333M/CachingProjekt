// CacheWindow.h
#pragma once
#include "../CacheInterface.h"
#include <QWidget>

class CacheWindow : public QWidget {
    // Q_OBJECT
private:
    CacheInterface *cache = nullptr;

public:
    CacheWindow() {};
    void setCache(CacheInterface *cacheInterface);
    void refresh() {};
};
