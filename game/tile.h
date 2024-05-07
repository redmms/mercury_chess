#pragma once
#include "virtual_tile.h"
#include <QLabel>

class QMouseEvent;
namespace mmd
{
    class Board;
    class Tile : public QLabel, public VirtualTile
    {
        Q_OBJECT
        QColor black;
        QColor white;
        QColor selected;
        QColor valid;
        QColor hover;
        std::map<std::string, std::map<int, QString>> css_colors;
        QString css;

        void mousePressEvent(QMouseEvent* event);
        static QColor mixColors(QColor color_a, QColor color_b, float b_coef);

    public:
        Tile(scoord tile_coord_ = {}, char piece_name_ = 'e', bool piece_color_ = false, bool side_ = true, Board* parent_ = 0);

        void setPiece(char elem, bool color, bool virtually = false) override;
        void dyeNormal();
        void dyeSelected();
        void dyeValid();
        VirtualTile toVirtu();

    signals:
        void tileClicked(Tile* tile);
    };
}  // namespace mmd