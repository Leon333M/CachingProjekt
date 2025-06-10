// MainWindow.cpp
#include "MainWindow.h"

MainWindow::MainWindow(ConfigLoader *controller)
    : configLoader(controller) {
    this->resize(400, 300);
    this->setWindowTitle("Testfenster");
    this->show();
}
