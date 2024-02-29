#pragma once
#include <QScrollArea>
#include <QPainter>
#include <QPaintEvent>

class RoundedScrollArea : public QScrollArea {
    QColor background_color;

public:
    RoundedScrollArea(QWidget *parent = nullptr, QColor background_color_ = Qt::white) :
        QScrollArea(parent),
        background_color(background_color_)
    {}

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(viewport());
        int radius = 14;
        painter.setBrush(background_color); // QColor(0, 102, 51)
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(viewport()->rect(), radius, radius);
    }
};