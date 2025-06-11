// MainWindow.cpp
#include "MainWindow.h"
#include "CacheWindow.h"

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

    leftLayout->addWidget(new QLabel("test leftLayout"));
    reigthtLayout->addWidget(new QLabel("test reigthtLayout"));

    this->resize(400, 300);
    this->setWindowTitle("Testfenster");
    this->show();
}
