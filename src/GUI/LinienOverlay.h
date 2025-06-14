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
        // PLOG_DEBUG << "Zeichne LinienOverlay anfang";
        QPainter p(this);
        p.fillRect(rect(), QColor(0, 255, 0, 100));
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(Qt::black, 2));

        for (auto &[von, nach] : verbindungen) {
            if (!von || !nach)
                continue;
            if (!von->isVisible() || !nach->isVisible())
                continue;
            if (!von->parentWidget() || !nach->parentWidget())
                continue;
            if (!isAncestorOf(von) || !isAncestorOf(nach)) {
                // PLOG_DEBUG << "WARNUNG: von/nach nicht Teil des Widget-Baums!" << isAncestorOf(von) << " " << isAncestorOf(nach);
            }
            // QPoint start = this->mapFromGlobal(von->mapToGlobal(von->rect().center()));
            // QPoint end = this->mapFromGlobal(nach->mapToGlobal(nach->rect().center()));

            QPointF vonCenter = von->mapToGlobal(von->rect().center());
            QPointF nachCenter = nach->mapToGlobal(nach->rect().center());

            QPointF startGlobal = borderPoint(von, nachCenter);
            QPointF endGlobal = borderPoint(nach, vonCenter);

            QPoint start = this->mapFromGlobal(startGlobal.toPoint());
            QPoint end = this->mapFromGlobal(endGlobal.toPoint());

            drawArrow(p, start, end);
        }
        // PLOG_DEBUG << "Zeichne LinienOverlay ende";
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

    QPointF borderPoint(const QWidget *widget, const QPointF &targetGlobal) {
        QRectF rect = widget->rect();
        QPointF centerGlobal = widget->mapToGlobal(rect.center());

        QPointF dir = targetGlobal - centerGlobal;

        if (dir == QPointF(0, 0))
            return centerGlobal;

        // Verhältnis für X- und Y-Rand bestimmen (distance to border along x- and y-axis)
        double halfWidth = rect.width() / 2.0;
        double halfHeight = rect.height() / 2.0;

        // Skaliere den Vektor so, dass er an den Rand des Rechtecks stößt
        double scaleX = halfWidth / std::abs(dir.x());
        double scaleY = halfHeight / std::abs(dir.y());

        double scale = std::min(scaleX, scaleY);

        QPointF border = centerGlobal + dir * scale;

        return border;
    }
};
