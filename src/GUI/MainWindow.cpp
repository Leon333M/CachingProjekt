// MainWindow.cpp
#include "MainWindow.h"
#include <plog/Log.h>

MainWindow::MainWindow(ConfigLoader *controller)
    : configLoader(controller),
      window(this),
      windowLayout(QGridLayout(&window)),
      vhddsWidget(&window),
      cachesWidget(&window),
      overlay(&window) {
    this->setCentralWidget(&window);
    this->resize(400, 300);
    this->setWindowTitle("Testfenster");
    this->show();

    // windowLayout.addWidget(&vhddsWidget, 0, 0);
    // windowLayout.addWidget(&cachesWidget, 0, 1);

    if (controller == nullptr) {
        return;
    }

    // init Vhdd's
    std::vector<VirtuelleFestplatte> *vhdds = controller->getVhdds();
    std::vector<VhddPair> vhddPair;
    for (VirtuelleFestplatte &vhdd : *vhdds) {
        VhddPair vhd(&vhdd, new VhddWindow(&vhdd));
        vhd.vhddWidget->setParent(&window);
        vhd.vhddWidget->setMaximumSize(200, 50);
        vhddPair.push_back(vhd);
    }

    // init Cache's
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> *cachesM = controller->getCacheMap();
    std::vector<CacheInterface *> caches;
    std::vector<CachePair> firstCachePair;
    std::vector<CachePair> cachePair;
    for (const auto &[name, cachePtr] : *cachesM) {
        // name (std::string), cache (std::shared_ptr<CacheInterface>)
        CacheInterface *cache = cachePtr.get();
        caches.push_back(cache);
    }
    for (CacheInterface *cache : caches) {
        CachePair cacheP(cache, new CacheWindow(cache));
        cacheP.cacheWidget->setParent(&window);
        cacheP.cacheWidget->setMaximumSize(200, 50);
        cachePair.push_back(cacheP);
    }

    // init overlay
    overlay.setGeometry(window.rect());
    overlay.raise();
    overlay.show();

    // layout
    windowLayout.setHorizontalSpacing(100);          // Abstand horizontal zwischen den Widgets
    windowLayout.setVerticalSpacing(100);            // Abstand vertikal zwischen den Widgets
    windowLayout.setContentsMargins(10, 10, 10, 10); // Außenabstand: links, oben, rechts, unten
    // windowLayout.setColumnStretch(0, 1);             // Spalte 0 kann wachsen
    // windowLayout.setColumnStretch(1, 1);             // Spalte 1 auch
    //  Je hoher der Wert, desto mehr Platz bekommt die Spalte

    // erstelle stuktur von vhdd
    for (int i = 0; i < vhddPair.size(); i++) {
        VhddPair vp = vhddPair.at(i);
        windowLayout.addWidget(vp.vhddWidget, i, 0);
        std::wstring cachName = vp.vhdd->getCache().getCacheName();
        CachePair cp = findeCachePairMitCacheName(cachName, cachePair);
        windowLayout.addWidget(cp.cacheWidget, i, 1);
        overlay.verbindungen.push_back({vp.vhddWidget, cp.cacheWidget});
        firstCachePair.push_back(cp);
    }

    // erstelle stuktur von Caches
    int t = 2;
    int z = 0;
    int zeile = 0;
    for (int i = 0; i < firstCachePair.size(); i++) {
        CachePair cp = firstCachePair.at(i);
        std::wstring cacheTyp = cp.cache->getCacheTyp();
        if (cacheTyp == L"Cache") {
            Cache *cache = static_cast<Cache *>(cp.cache);
            CacheInterface &c1 = cache->getRamCache();
            CacheInterface &c2 = cache->getSsdCache();
            CachePair cp1 = findeCachePairMitCacheName(c1.getCacheName(), cachePair);
            CachePair cp2 = findeCachePairMitCacheName(c2.getCacheName(), cachePair);
            windowLayout.addWidget(cp1.cacheWidget, z++, t);
            windowLayout.addWidget(cp2.cacheWidget, z++, t);
            overlay.verbindungen.push_back({cp.cacheWidget, cp1.cacheWidget});
            overlay.verbindungen.push_back({cp.cacheWidget, cp2.cacheWidget});
            int spalte = t + 1;
            baueCacheRekursivAuf(cp1, zeile, spalte, cachePair);
            baueCacheRekursivAuf(cp2, zeile, spalte, cachePair);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    overlay.resize(window.size());
    overlay.update();
}

CachePair MainWindow::findeCachePairMitCacheName(const std::wstring &cacheName, std::vector<CachePair> &cachePairs) {
    for (CachePair &pair : cachePairs) {
        CacheInterface *ci = pair.cache;
        if (ci->getCacheName() == cacheName) {
            return pair;
        }
    }
    // nie
    return cachePairs.at(0);
}

void MainWindow::baueCacheRekursivAuf(CachePair &cp, int &zeile, int spalte, std::vector<CachePair> &alleCachePairs) {

    // Nur weitergehen, wenn es ein zusammengesetzter Cache ist
    if (cp.cache->getCacheTyp() == L"Cache") {
        Cache *komplexerCache = static_cast<Cache *>(cp.cache);

        CacheInterface &ramRef = komplexerCache->getRamCache();
        CacheInterface &ssdRef = komplexerCache->getSsdCache();

        CachePair ramPair = findeCachePairMitCacheName(ramRef.getCacheName(), alleCachePairs);
        CachePair ssdPair = findeCachePairMitCacheName(ssdRef.getCacheName(), alleCachePairs);

        // Widget platzieren
        windowLayout.addWidget(ramPair.cacheWidget, zeile, spalte);
        zeile++;
        windowLayout.addWidget(ssdPair.cacheWidget, zeile, spalte);
        zeile++;

        overlay.verbindungen.push_back({cp.cacheWidget, ramPair.cacheWidget});
        overlay.verbindungen.push_back({cp.cacheWidget, ssdPair.cacheWidget});

        // Rekursiv weiter (in nachster Zeile)
        int z = 0;
        baueCacheRekursivAuf(ramPair, z, spalte + 1, alleCachePairs);
        baueCacheRekursivAuf(ssdPair, z, spalte + 1, alleCachePairs);
    }
}
