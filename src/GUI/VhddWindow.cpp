// VhddWindow.cpp
#include "VhddWindow.h"

VhddWindow::VhddWindow(VirtuelleFestplatte *vhd)
    : windowLayout(QGridLayout(this)) {
    vhdd = vhd;
    int i = 0;
    windowLayout.addWidget(&labelVhddOrginalVolume, 0, i++);
    windowLayout.addWidget(&labelVhddVolume, 0, i++);
    windowLayout.addWidget(&labelCacheName, 0, i++);
    refresh();
}

void VhddWindow::refresh() {
    labelVhddOrginalVolume.setText(QString::fromWCharArray(vhdd->getOrginalVolume().c_str()));
    labelVhddVolume.setText(QString::fromWCharArray(vhdd->getNeuesVolume().c_str()));
    labelCacheName.setText(QString::fromUtf8(vhdd->getCache().getCacheTyp()));
}
