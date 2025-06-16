// KreisWidget.h
#pragma once
#include <QList>
#include <QWidget>
#include <cmath>

class KreisWidget : public QWidget {
    // Q_OBJECT

public:
    KreisWidget(QWidget *parent = nullptr);

    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);
    void clearWidgets();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QList<QWidget *> widgets;
    void updatePositionen();
};
