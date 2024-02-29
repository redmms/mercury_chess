#pragma once
#include <QScrollArea>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QScrollBar>

class HorizontalScrollArea : public QScrollArea {
    QColor background_color;

public:
    HorizontalScrollArea(QWidget* parent = nullptr, QColor background_color_ = Qt::white) :
        QScrollArea(parent),
        background_color(background_color_)
    {}

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);

        QPainter painter(viewport());
        int radius = 14;
        painter.setBrush(background_color); // QColor(0, 102, 51)
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(viewport()->rect(), radius, radius);
    }
    void wheelEvent(QWheelEvent* event) override {
        if (event->angleDelta().y() > 0) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalScrollBar()->singleStep());
        }
        else {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - horizontalScrollBar()->singleStep());
        }
        QScrollArea::wheelEvent(event);
    }
};