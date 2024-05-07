#include "history_area.h"
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QLabel>
#include <QLayout>
using namespace std;

namespace mmd
{
    HistoryArea::HistoryArea(QWidget* parent_, QColor background_color_) :
        QScrollArea(parent_),
        background_color(background_color_),
        history_label(new QLabel(this))
    {
        history_label->setStyleSheet("background-color: transparent;");
        history_label->setFont({ "Segoe UI", 12 });
        history_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        setWidget(history_label);
        setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        setWidgetResizable(true);
    }

    void HistoryArea::writeStory(size_t order, halfmove hmove) {
        QString out = halfmoveToString(hmove);
        if (order % 2)
            history_label->setText(history_label->text() + QString::number((order - 1) / 2 + 1) + ". " + out + " ");
        else
            history_label->setText(history_label->text() + out + " ");
        history_label->adjustSize();
        horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
    }

    void HistoryArea::clearStory() {
        history_label->clear();
        history_label->adjustSize();
    }

    void HistoryArea::paintEvent(QPaintEvent* event) {
        Q_UNUSED(event);

        QPainter painter(viewport());
        int radius = 14;
        painter.setBrush(background_color);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(viewport()->rect(), radius, radius);
    }

    void HistoryArea::wheelEvent(QWheelEvent* event) {
        if (event->angleDelta().y() > 0) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalScrollBar()->singleStep());
        }
        else {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - horizontalScrollBar()->singleStep());
        }
        QScrollArea::wheelEvent(event);
    }
}  // namespace mmd