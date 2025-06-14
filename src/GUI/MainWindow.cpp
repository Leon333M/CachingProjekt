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
    // erstelle stuktur von vhdd
    for (int i = 0; i < vhddPair.size(); i++) {
        VhddPair vp = vhddPair.at(i);
        windowLayout.addWidget(vp.vhddWidget, i, 0);
        std::wstring cachName = vp.vhdd->getCache().getCacheName();
        CachePair cp = findeCachePairMitCacheName(cachName, cachePair);
        windowLayout.addWidget(cp.cacheWidget, i, 1);
        overlay.verbindungen.push_back({vp.vhddWidget, cp.cacheWidget});
    }

    /*
    // Vhdd's
    std::vector<QWidget *> vhddsWidgets;
    for (VirtuelleFestplatte &vhdd : *vhdds) {
    }
    for (int i = 0; i < vhdds->size(); i++) {
        VirtuelleFestplatte &vhdd = vhdds->at(i);
        VhddWindow *vhddWidget = new VhddWindow(&vhdd);
        vhddWidget->setParent(&window);
        vhddsWidgets.push_back(vhddWidget);
        windowLayout.addWidget(vhddWidget, i, 0);
    }
    // Cache's
    std::vector<QWidget *> cachesWidgets;
    for (int i = 0; i < caches.size(); i++) {
        CacheInterface *cache = caches.at(i);
        CacheWindow *cacheWidget = new CacheWindow(cache);
        cacheWidget->setParent(&window);
        cachesWidgets.push_back(cacheWidget);
        windowLayout.addWidget(cacheWidget, i, 1);
    }


    // layout
    windowLayout.setHorizontalSpacing(100);          // Abstand horizontal zwischen den Widgets
    windowLayout.setVerticalSpacing(100);            // Abstand vertikal zwischen den Widgets
    windowLayout.setContentsMargins(10, 10, 10, 10); // Außenabstand: links, oben, rechts, unten
    // windowLayout.setColumnStretch(0, 1);             // Spalte 0 kann wachsen
    // windowLayout.setColumnStretch(1, 1);             // Spalte 1 auch
    //  Je hoher der Wert, desto mehr Platz bekommt die Spalte
    // for (QWidget *widget : vhddsWidgets) {
    //    widget->setMaximumSize(200, 50);
    //}

    // Zeiche pfeile.
    overlay.verbindungen.push_back({vhddsWidgets.at(0), vhddsWidgets.at(1)});
    overlay.verbindungen.push_back({vhddsWidgets.at(0), cachesWidgets.at(0)});
    overlay.show();
    */
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
