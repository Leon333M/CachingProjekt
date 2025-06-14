// LinienOverlay.h
#pragma once
#include <QPainter>
#include <QWidget>
#include <cmath>
#include <vector>

#include <plog/Log.h>

class LinienOverlay : public QWidget {
public:
    std::vector<std::pair<QWidget *, QWidget *>> verbindungen;

    LinienOverlay(QWidget *parent) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_AlwaysStackOnTop);
        setStyleSheet("background: transparent;");
    }

    void paintEvent(QPaintEvent *event) {
        PLOG_DEBUG << "Zeichne LinienOverlay anfang";
        QPainter p(this);
        p.fillRect(rect(), QColor(0, 255, 0, 100));
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(Qt::black, 2));

        PLOG_DEBUG << "Zeichne LinienOverlay 2";

        for (auto &[von, nach] : verbindungen) {
            if (!von || !nach)
                continue;
            if (!von->isVisible() || !nach->isVisible())
                continue;
            if (!von->parentWidget() || !nach->parentWidget())
                continue;
            if (!isAncestorOf(von) || !isAncestorOf(nach)) {
                PLOG_DEBUG << "WARNUNG: von/nach nicht Teil des Widget-Baums!" << isAncestorOf(von) << " " << isAncestorOf(nach);
                QPoint start = this->mapFromGlobal(von->mapToGlobal(von->rect().center()));
                QPoint end = this->mapFromGlobal(nach->mapToGlobal(nach->rect().center()));
                drawArrow(p, start, end);
            } else {
                QPoint start = von->mapTo(this, von->rect().center());
                QPoint end = nach->mapTo(this, nach->rect().center());
                drawArrow(p, start, end);
            }
        }
        PLOG_DEBUG << "Zeichne LinienOverlay ende";
    }

private:
    void drawArrow(QPainter &p, const QPoint &start, const QPoint &end) {
        p.drawLine(start, end);
        // kleiner Pfeilkopf
        QPointF dir = (end - start);
        dir = dir / std::sqrt(QPointF::dotProduct(dir, dir)) * 10.0;
        QPointF ortho(-dir.y(), dir.x());
        QPointF arrowP1 = end - dir + ortho;
        QPointF arrowP2 = end - dir - ortho;
        QPolygonF head;
        head << end << arrowP1 << arrowP2;
        p.drawPolygon(head);
    }
};
