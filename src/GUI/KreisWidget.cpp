// KreisWidget.cpp
#include "KreisWidget.h"

KreisWidget::KreisWidget(QWidget *parent)
    : QWidget(parent) {
}

void KreisWidget::addWidget(QWidget *widget) {
    if (!widget || widgets.contains(widget))
        return;

    widget->setParent(this);
    widgets.append(widget);
    widget->show();
    updatePositionen();
}

void KreisWidget::removeWidget(QWidget *widget) {
    if (widgets.removeOne(widget)) {
        widget->hide();
        widget->setParent(nullptr);
        updatePositionen();
    }
}

void KreisWidget::clearWidgets() {
    for (QWidget *w : widgets) {
        w->hide();
        w->setParent(nullptr);
    }
    widgets.clear();
    updatePositionen();
}

void KreisWidget::resizeEvent(QResizeEvent *) {
    updatePositionen();
}

void KreisWidget::updatePositionen() {
    const int n = widgets.size();
    if (n == 0)
        return;

    const int radius = qMin(width(), height()) / 2 - 40; // 40 = Abstand zur Kante
    const QPoint center(width() / 2, height() / 2);

    for (int i = 0; i < n; ++i) {
        double angle = 2.0 * M_PI * i / n;
        int x = center.x() + radius * std::cos(angle) - widgets[i]->width() / 2;
        int y = center.y() + radius * std::sin(angle) - widgets[i]->height() / 2;

        widgets[i]->move(x, y);
    }
}
