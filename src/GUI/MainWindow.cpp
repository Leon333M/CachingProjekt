// MainWindow.cpp
#include "MainWindow.h"
#include "CacheWindow.h"
#include <plog/Log.h>

MainWindow::MainWindow(ConfigLoader *controller)
    : configLoader(controller),
      window(),
      windowLayout(QGridLayout(&window)),
      vhddsWidget(),
      cachesWidget() {
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
    leftLayout->addWidget(new QLabel("test leftLayout"));

    // Cache's
    std::unordered_map<std::string, std::shared_ptr<CacheInterface>> *cachesM = controller->getCacheMap();
    for (const auto &[name, cachePtr] : *cachesM) {
        // name (std::string), cache (std::shared_ptr<CacheInterface>)
        CacheInterface *cache = cachePtr.get();
        PLOG_DEBUG << name;
        reigthtLayout->addWidget(new CacheWindow(name, cache));
    }
}
