// AutoRowGridLayout.h
#pragma once
#include <QApplication>
#include <QGridLayout>
#include <plog/Log.h>

class AutoRowGridLayout : public QGridLayout {
    std::map<int, int> currentRowPerColumn;

public:
    AutoRowGridLayout(QWidget *parent = nullptr)
        : QGridLayout(parent) {}

    void addWidgetAutoRow(QWidget *widget, int column = 0) {
        int row = currentRowPerColumn[column]++;
        addWidget(widget, row, column);
        PLOG_DEBUG << "Saplate: " << column << " Zeile: " << row;
    }
};