// VhddWindow.h
#pragma once
#include "../VirtuelleFestplatte.h"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

class VhddWindow : public QWidget {
private:
    VirtuelleFestplatte *vhdd = nullptr;
    std::string vhddName;
    QGridLayout windowLayout;
    QLabel labelVhddOrginalVolume;
    QLabel labelVhddVolume;
    QLabel labelCacheName;

public:
    VhddWindow(VirtuelleFestplatte *vhd);
    void refresh();

private:
};
