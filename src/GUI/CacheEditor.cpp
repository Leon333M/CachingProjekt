// CacheEditor.cpp
#include "CacheEditor.h"
#include "../Cache.h"
#include <QPainter>
#include <plog/Log.h>

CacheEditor::CacheEditor(CacheInterface *cacheInterface, QIcon &icon)
    : cache(cacheInterface),
      windowLayout(QGridLayout(this)) {
    this->setWindowIcon(icon);

    QGridLayout *layout0 = new QGridLayout();
    layout0->addWidget(new QLabel("CacheName:"), 0, 0);
    layout0->addWidget(new QLabel(QString::fromWCharArray(cacheInterface->getCacheName().c_str())), 0, 1);
    windowLayout.addLayout(layout0, 0, 0);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(new QLabel("CacheTyp:"), 0, 0);
    layout1->addWidget(&labelCacheTyp, 0, 1);
    windowLayout.addLayout(layout1, 1, 0);

    if (cache->getCacheTyp() != L"Cache") {
        QGridLayout *layout2 = new QGridLayout();
        layout2->addWidget(new QLabel("CacheUsage:"), 0, 0);
        layout2->addWidget(&labelCacheUsage, 0, 1);
        windowLayout.addLayout(layout2, 2, 0);

        QGridLayout *layout3 = new QGridLayout();
        layout3->addWidget(new QLabel("CurrentCacheSize:"), 0, 0);
        layout3->addWidget(&labelCurrentCacheSize, 0, 1);
        windowLayout.addLayout(layout3, 3, 0);

        QGridLayout *layout4 = new QGridLayout();
        layout4->addWidget(new QLabel("MaxCacheSize:"), 0, 0);
        layout4->addWidget(&labelMaxCacheSize, 0, 1);
        windowLayout.addLayout(layout4, 4, 0);

        QGridLayout *layout5 = new QGridLayout();
        layout5->addWidget(new QLabel("MinZugriffsHaufigkeit:"), 0, 0);
        layout5->addWidget(&labelMinZugriffsHaufigkeit, 0, 1);
        windowLayout.addLayout(layout5, 5, 0);

        QGridLayout *layout6 = new QGridLayout();
        layout6->addWidget(new QLabel("MaxPfadHistorie:"), 0, 0);
        layout6->addWidget(new QLabel(QString::number(cache->getMaxPfadHistorie())), 0, 1);
        windowLayout.addLayout(layout6, 6, 0);

        QGridLayout *layout7 = new QGridLayout();
        layout7->addWidget(new QLabel("CashePfade:"), 0, 0);
        layout7->addWidget(&cashePfadeTextBlock, 1, 0);
        cashePfadeTextBlock.setReadOnly(true);
        cashePfadeTextBlock.setFrameStyle(QFrame::NoFrame);
        cashePfadeTextBlock.setStyleSheet(
            "QPlainTextEdit {"
            "  border: 1px solid black;" // schwarzer Rahmen, 1 Pixel dick
            "  background: transparent;" // Hintergrund transparent
            "  color: black;"            // Textfarbe
            "}");
        windowLayout.addLayout(layout7, 7, 0);

        if (cache->getCacheTyp() == L"SsdCache") {
            int abZeile = 8;

            SsdCache *cache0 = static_cast<SsdCache *>(cache);
            QGridLayout *layout1 = new QGridLayout();
            layout1->addWidget(new QLabel("CacheVolume:"), 0, 0);
            layout1->addWidget(new QLabel(QString::fromWCharArray(cache0->getCacheVolume().c_str())), 0, 1);
            windowLayout.addLayout(layout1, abZeile++, 0);

            QGridLayout *layout2 = new QGridLayout();
            layout2->addWidget(new QLabel("CacheStammVerzeichnis:"), 0, 0);
            layout2->addWidget(new QLabel(QString::fromWCharArray(cache0->getCacheStammVerzeichnis().c_str())), 0, 1);
            windowLayout.addLayout(layout2, abZeile++, 0);
        }
    } else {
        Cache *cache0 = static_cast<Cache *>(cache);

        QGridLayout *layout3 = new QGridLayout();
        layout3->addWidget(new QLabel("priorityCache:"), 0, 0);
        layout3->addWidget(new QLabel(QString::fromWCharArray(cache0->getRamCache().getCacheName().c_str())), 0, 1);
        windowLayout.addLayout(layout3, 2, 0);

        QGridLayout *layout4 = new QGridLayout();
        layout4->addWidget(new QLabel("secondaryCache:"), 0, 0);
        layout4->addWidget(new QLabel(QString::fromWCharArray(cache0->getSsdCache().getCacheName().c_str())), 0, 1);
        windowLayout.addLayout(layout4, 3, 0);
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
        labelCacheUsage.setText(QString::number(cache->getCurrentCacheSize() / (cache->getMaxCacheSize() / 100)) + "%");
        labelCurrentCacheSize.setText(QString::number(cache->getCurrentCacheSize()));
        labelMaxCacheSize.setText(QString::number(cache->getMaxCacheSize()));
        labelMinZugriffsHaufigkeit.setText(QString::number(cache->getMinZugriffsHaufigkeit()));
        QString text;
        std::unordered_set<std::wstring> pfadListe = cache->getCashePfade();
        for (const std::wstring &pfad : pfadListe) {
            text += QString::fromWCharArray(pfad.c_str()) + QChar::LineFeed;
        }
        cashePfadeTextBlock.setPlainText(text);
    }
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

void CacheEditor::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(0, 255, 0, 100));
}
