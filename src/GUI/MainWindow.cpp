// MainWindow.cpp
#include "MainWindow.h"
#include <plog/Log.h>

MainWindow::MainWindow(ConfigLoader *controller)
    : configLoader(controller),
      window(this),
      windowLayout(AutoRowGridLayout(&window)),
      overlay(&window) {
    this->setCentralWidget(&window);
    this->resize(400, 300);
    this->setWindowTitle("CachingProjekt");

    if (controller == nullptr) {
        return;
    }

    // init Vhdd's
    std::vector<VirtuelleFestplatte> *vhdds = controller->getVhdds();
    std::vector<VhddPair> vhddPair;
    for (VirtuelleFestplatte &vhdd : *vhdds) {
        VhddPair vhd(&vhdd, new VhddWindow(&vhdd));
        vhd.vhddWidget->setParent(&window);
        vhd.vhddWidget->setMaximumSize(300, 50);
        vhddPair.push_back(vhd);
    }

    // init Cache's
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> *cachesM = controller->getCacheMap();
    std::vector<CacheInterface *> caches;
    for (const auto &[name, cachePtr] : *cachesM) {
        // name (std::string), cache (std::shared_ptr<CacheInterface>)
        CacheInterface *cache = cachePtr.get();
        caches.push_back(cache);
    }
    for (CacheInterface *cache : caches) {
        CachePair cacheP(cache, new CacheWindow(cache));
        cacheP.cacheWidget->setParent(&window);
        cacheP.cacheWidget->setMaximumSize(300, 50);
        cachePair.push_back(cacheP);
    }

    // layout
    windowLayout.setHorizontalSpacing(100);          // Abstand horizontal zwischen den Widgets
    windowLayout.setVerticalSpacing(100);            // Abstand vertikal zwischen den Widgets
    windowLayout.setContentsMargins(10, 10, 10, 10); // Außenabstand: links, oben, rechts, unten
    // windowLayout.setColumnStretch(0, 1);          // Spalte 0 kann wachsen
    // windowLayout.setColumnStretch(1, 1);          // Spalte 1 auch
    // Je hoher der Wert, desto mehr Platz bekommt die Spalte

    // erstelle stuktur von vhdd
    for (int i = 0; i < vhddPair.size(); i++) {
        VhddPair vp = vhddPair.at(i);
        windowLayout.addWidgetAutoRow(vp.vhddWidget, 0);
        std::wstring cachName = vp.vhdd->getCache().getCacheName();
        CachePair &cp = findeCachePairMitCacheName(cachName);
        if (!cp.isAdded) {
            cp.isAdded = true;
            windowLayout.addWidgetAutoRow(cp.cacheWidget, 1);
        }
        overlay.verbindungen.push_back({vp.vhddWidget, cp.cacheWidget});
        std::wstring cacheTyp = cp.cache->getCacheTyp();
        if (cacheTyp == L"Cache") {
            baueCacheRekursivAuf(cp, 2);
        }
    }

    // Liste ungenutze Caches auf
    std::vector<CachePair> unusedCachePairs;
    for (CachePair &pair : cachePair) {
        if (!pair.isAdded) {
            unusedCachePairs.emplace_back(pair);
        }
    }

    // zeige ungenutzte caches an
    for (CachePair &pair : unusedCachePairs) {
        CacheInterface *cacheI = pair.cache;
        if (cacheI->getCacheTyp() == L"Cache") {
            baueCacheRekursivAuf(findeCachePairMitCacheName(cacheI->getCacheName()), 0);
        }
    }
    for (CachePair &pair : unusedCachePairs) {
        CacheInterface *cacheI = pair.cache;
        baueCacheRekursivAuf(findeCachePairMitCacheName(cacheI->getCacheName()), 0);
    }

    if (configLoader->getHideGui()) {
        this->hide();
    } else {
        this->show();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    overlay.resize(window.size());
    overlay.update();
}

CachePair &MainWindow::findeCachePairMitCacheName(const std::wstring &cacheName) {
    for (CachePair &pair : cachePair) {
        CacheInterface *ci = pair.cache;
        if (ci->getCacheName() == cacheName) {
            return pair;
        }
    }
    // nie
    return cachePair.at(0);
}

void MainWindow::baueCacheRekursivAuf(CachePair &cp, int spalte) {
    if (spalte < 1) {
        spalte = 1;
    }
    // Widget platzieren
    if (!cp.isAdded) {
        cp.isAdded = true;
        windowLayout.addWidgetAutoRow(cp.cacheWidget, spalte - 1);
    }

    // Nur weitergehen, wenn es ein zusammengesetzter Cache ist
    if (cp.cache->getCacheTyp() == L"Cache") {
        Cache *komplexerCache = static_cast<Cache *>(cp.cache);

        CacheInterface &ramRef = komplexerCache->getRamCache();
        CacheInterface &ssdRef = komplexerCache->getSsdCache();

        CachePair &ramPair = findeCachePairMitCacheName(ramRef.getCacheName());
        CachePair &ssdPair = findeCachePairMitCacheName(ssdRef.getCacheName());

        // Widget platzieren
        if (!ramPair.isAdded) {
            ramPair.isAdded = true;
            windowLayout.addWidgetAutoRow(ramPair.cacheWidget, spalte);
        }
        if (!ssdPair.isAdded) {
            ssdPair.isAdded = true;
            windowLayout.addWidgetAutoRow(ssdPair.cacheWidget, spalte);
        }

        overlay.verbindungen.push_back({cp.cacheWidget, ramPair.cacheWidget, Qt::blue});
        overlay.verbindungen.push_back({cp.cacheWidget, ssdPair.cacheWidget, Qt::yellow});

        // Rekursiv weiter (in nachster spalte)
        baueCacheRekursivAuf(ramPair, spalte + 1);
        baueCacheRekursivAuf(ssdPair, spalte + 1);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (qApp->closingDown()) {
        // App beendet sich
        event->accept();
    } else {
        // Sonst nur verstecken
        this->hide();
        event->ignore();
    }
}
