#pragma once
#include "../game/virtual_tile.h"
#include <QScrollArea>
#include <QColor>

class QPaintEvent;
class QLabel;
namespace mmd
{
    class HistoryArea : public QScrollArea 
    {
        QColor background_color;
        QLabel* history_label;

    public:
        HistoryArea(QWidget* parent_, QColor background_color_ = Qt::white);

        void writeStory(int order, halfmove hmove);
        void clearStory();

    protected:
        void paintEvent(QPaintEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
    };
}