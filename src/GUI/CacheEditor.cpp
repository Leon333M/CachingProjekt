// CacheEditor.cpp
#include "CacheEditor.h"
#include <plog/Log.h>

CacheEditor::CacheEditor(CacheInterface *cacheInterface)
    : cache(cacheInterface),
      windowLayout(QGridLayout(this)) {
    windowLayout.addWidget(new QLabel(QString::fromWCharArray(cacheInterface->getCacheName().c_str())));
    windowLayout.addWidget(&labelCacheTyp);
    windowLayout.addWidget(&labelCurrentCacheSize);
    windowLayout.addWidget(&labelMaxCacheSize);
    windowLayout.addWidget(&labelMinZugriffsHaufigkeit);
    refresh();
}

CacheEditor::~CacheEditor() {
    if (cache->getCacheTyp() != L"Cache") {
        cache->entferneListener(benachrichtigerEventId);
    }
}

void CacheEditor::refresh() {
    if (cache->getCacheTyp() != L"Cache") {
        labelCacheTyp.setText(QString::fromWCharArray(cache->getCacheTyp().c_str()));
        labelCurrentCacheSize.setText(QString::number(cache->getCurrentCacheSize()));
        labelMaxCacheSize.setText(QString::number(cache->getMaxCacheSize()));
        labelMinZugriffsHaufigkeit.setText(QString::number(cache->getMinZugriffsHaufigkeit()));
    }
}

void CacheEditor::showEditor() {
    if (cache->getCacheTyp() != L"Cache") {
        benachrichtigerEventId = cache->registriereListener([this]() {
            QMetaObject::invokeMethod(this, [this]() { refresh(); }, Qt::QueuedConnection);
        });
    }
    refresh();
    show();
}

void CacheEditor::hideEditor() {
    if (cache->getCacheTyp() != L"Cache") {
        cache->entferneListener(benachrichtigerEventId);
    }
    hide();
}

void CacheEditor::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    refresh();
    if (cache->getCacheTyp() != L"Cache") {
        benachrichtigerEventId = cache->registriereListener([this]() {
            QMetaObject::invokeMethod(this, [this]() { refresh(); }, Qt::QueuedConnection);
        });
    }
}

void CacheEditor::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    if (cache->getCacheTyp() != L"Cache") {
        cache->entferneListener(benachrichtigerEventId);
    }
}
