// CacheEditor.cpp
#include "CacheEditor.h"
#include <plog/Log.h>

CacheEditor::CacheEditor(CacheInterface *cacheInterface)
    : cache(cacheInterface),
      windowLayout(QGridLayout(this)) {
    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(new QLabel("CacheName:"), 0, 0);
    layout1->addWidget(new QLabel(QString::fromWCharArray(cacheInterface->getCacheName().c_str())), 0, 1);
    windowLayout.addLayout(layout1, 0, 0);
    QGridLayout *layout2 = new QGridLayout();
    layout2->addWidget(new QLabel("CacheTyp:"), 0, 0);
    layout2->addWidget(&labelCacheTyp, 0, 1);
    windowLayout.addLayout(layout2, 1, 0);
    if (cache->getCacheTyp() != L"Cache") {
        QGridLayout *layout3 = new QGridLayout();
        layout3->addWidget(new QLabel("CurrentCacheSize:"), 0, 0);
        layout3->addWidget(&labelCurrentCacheSize, 0, 1);
        windowLayout.addLayout(layout3, 2, 0);
        QGridLayout *layout4 = new QGridLayout();
        layout4->addWidget(new QLabel("MaxCacheSize:"), 0, 0);
        layout4->addWidget(&labelMaxCacheSize, 0, 1);
        windowLayout.addLayout(layout4, 3, 0);
        QGridLayout *layout5 = new QGridLayout();
        layout5->addWidget(new QLabel("MinZugriffsHaufigkeit:"), 0, 0);
        layout5->addWidget(&labelMinZugriffsHaufigkeit, 0, 1);
        windowLayout.addLayout(layout5, 4, 0);
    }
    refresh();
}

CacheEditor::~CacheEditor() {
    if (cache->getCacheTyp() != L"Cache") {
        cache->entferneListener(benachrichtigerEventId);
    }
}

void CacheEditor::refresh() {
    labelCacheTyp.setText(QString::fromWCharArray(cache->getCacheTyp().c_str()));
    if (cache->getCacheTyp() != L"Cache") {
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
