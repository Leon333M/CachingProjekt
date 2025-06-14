// MainWindow.cpp
#include "MainWindow.h"
#include "CacheWindow.h"
#include "VhddWindow.h"
#include <plog/Log.h>

MainWindow::MainWindow(ConfigLoader *controller)
    : configLoader(controller),
      window(this),
      windowLayout(QGridLayout(&window)),
      vhddsWidget(&window),
      cachesWidget(&window),
      overlay(&window) {
    this->setCentralWidget(&window);

    windowLayout.addWidget(&vhddsWidget, 0, 0);
    windowLayout.addWidget(&cachesWidget, 0, 1);

    QGridLayout *leftLayout = new QGridLayout(&vhddsWidget);
    QGridLayout *reigthtLayout = new QGridLayout(&cachesWidget);

    this->resize(400, 300);
    this->setWindowTitle("Testfenster");
    this->show();

    if (controller == nullptr) {
        return;
    }

    // leftLayout
    leftLayout->setHorizontalSpacing(100);          // Abstand horizontal zwischen den Widgets
    leftLayout->setVerticalSpacing(100);            // Abstand vertikal zwischen den Widgets
    leftLayout->setContentsMargins(10, 10, 10, 10); // Außenabstand: links, oben, rechts, unten
    // leftLayout->setColumnStretch(0, 1);             // Spalte 0 kann wachsen
    // leftLayout->setColumnStretch(1, 1);             // Spalte 1 auch
    //  Je hoher der Wert, desto mehr Platz bekommt die Spalte

    // Vhdd's
    std::vector<QWidget *> vhddsWidgets;
    std::vector<VirtuelleFestplatte> *vhdds = controller->getVhdds();
    for (VirtuelleFestplatte &vhdd : *vhdds) {
        VhddWindow *vhddWidget = new VhddWindow(&vhdd);
        vhddWidget->setParent(&vhddsWidget);
        leftLayout->addWidget(vhddWidget);
        vhddsWidgets.push_back(vhddWidget);
    }

    // Cache's
    std::vector<QWidget *> cachesWidgets;
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> *cachesM = controller->getCacheMap();
    for (const auto &[name, cachePtr] : *cachesM) {
        // name (std::string), cache (std::shared_ptr<CacheInterface>)
        CacheInterface *cache = cachePtr.get();
        CacheWindow *cacheWidget = new CacheWindow(name, cache);
        cacheWidget->setParent(&cachesWidget);
        reigthtLayout->addWidget(cacheWidget);
        cachesWidgets.push_back(cacheWidget);
    }

    // layout
    for (QWidget *widget : vhddsWidgets) {
        widget->setMaximumSize(200, 50);
    }

    // Zeiche pfeile.
    overlay.setGeometry(window.rect());
    overlay.raise();
    overlay.verbindungen.push_back({vhddsWidgets.at(0), vhddsWidgets.at(1)});
    overlay.verbindungen.push_back({vhddsWidgets.at(0), cachesWidgets.at(0)});
    overlay.show();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    overlay.resize(window.size());
    overlay.update();
}
