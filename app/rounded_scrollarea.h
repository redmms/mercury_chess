#ifndef ROUNDED_SCROLLAREA_H
#define ROUNDED_SCROLLAREA_H
#include <QScrollArea>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>

class RoundedScrollArea : public QScrollArea {
public:
    RoundedScrollArea(QWidget *parent = nullptr) : QScrollArea(parent) {}

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(viewport());
        int radius = 14;
        painter.setBrush(QColor(0, 102, 51));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(viewport()->rect(), radius, radius);
    }
};

#endif // ROUNDED_SCROLLAREA_H
