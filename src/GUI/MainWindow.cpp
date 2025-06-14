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

    // Vhdd's
    std::vector<QWidget *> vhddsWidgets;
    std::vector<VirtuelleFestplatte> *vhdds = controller->getVhdds();
    for (VirtuelleFestplatte &vhdd : *vhdds) {
        VhddWindow *vhddWidget = new VhddWindow(&vhdd);
        vhddWidget->setParent(&vhddsWidget);
        leftLayout->addWidget(vhddWidget);
        vhddsWidgets.push_back(vhddWidget);
        PLOG_DEBUG << "vhddWidget->parent(): " << vhddWidget->parentWidget();
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

    // Zeiche pfeile.
    overlay.setGeometry(window.rect());
    overlay.raise();
    overlay.verbindungen.push_back({vhddsWidgets.at(0), vhddsWidgets.at(1)});
    overlay.show();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    overlay.resize(window.size());
    overlay.update();
}
