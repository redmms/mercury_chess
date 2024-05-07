#include "tile.h"
#include "board.h"
using namespace std;

namespace mmd
{
    Tile::Tile(scoord tile_coord_, char piece_name_, bool piece_color_, bool side_, Board* parent_) :
        QLabel(parent_),
        VirtualTile(tile_coord_, piece_name_, piece_color_, parent_),
        black{ 120, 120, 90 },
        white{ 211, 211, 158 },
        selected(Qt::green),
        valid(Qt::yellow),
        hover{ 185, 108, 146 }, // previous: {170, 85, 127}
        css("mmd--Tile{background-color: %0;}:hover{background-color: %1;}")
    {
        css_colors["normal"][0] = black.name(QColor::HexRgb);
        css_colors["normal"][1] = white.name(QColor::HexRgb);
        css_colors["selected"][0] = mixColors(black, selected, 0.5).name(QColor::HexRgb);
        css_colors["selected"][1] = mixColors(white, selected, 0.5).name(QColor::HexRgb);
        css_colors["valid"][0] = mixColors(black, valid, 0.5).name(QColor::HexRgb);
        css_colors["valid"][1] = mixColors(white, valid, 0.5).name(QColor::HexRgb);
        css_colors["hover"][0] = hover.name(QColor::HexRgb);
        css_colors["hover"][1] = hover.name(QColor::HexRgb);

        dyeNormal();
        int size = parent_->TileSize();
        if (side_)
            setGeometry(size / 2 + coord.x * size, size / 2 + (7 - coord.y) * size,
                size, size); // size/2 is the indent from the left upper corner
        else
            setGeometry(size / 2 + (7 - coord.x) * size, size / 2 + coord.y * size,
                size, size);

        setPiece(piece_name_, piece_color_);
    }

    void Tile::mousePressEvent(QMouseEvent* event)
    {
        emit tileClicked(this);
    }

    QColor Tile::mixColors(QColor color_a, QColor color_b, float b_coef)
    {
        return QColor(
            color_a.red() * (1 - b_coef) + color_b.red() * b_coef,
            color_a.green() * (1 - b_coef) + color_b.green() * b_coef,
            color_a.blue() * (1 - b_coef) + color_b.blue() * b_coef,
            255);
    }

    void Tile::setPiece(char name, bool color, bool virtually)
    {
        VirtualTile::setPiece(name, color);  // 3d parameter is default
        if (virtually) {
            return;
        }
        if (name == 'e') {
            clear();
            dyeNormal();
            return;
        }
        QString add = color ? "white" : "black";
        QString piece;
        switch (name)
        {
        case 'P':
            piece = ":images/pawn_" + add;
            break;
        case 'R':
            piece = ":images/rook_" + add;
            break;
        case 'N':
            piece = ":images/knight_" + add;
            break;
        case 'K':
            piece = ":images/king_" + add;
            break;
        case 'Q':
            piece = ":images/queen_" + add;
            break;
        case 'B':
            piece = ":images/bishop_" + add;
        }
        setPixmap(piece);
    }

    void Tile::dyeNormal()
    {
        setStyleSheet(css.arg(css_colors["normal"][tile_color]).arg(css_colors["hover"][tile_color]));
    }

    void Tile::dyeSelected()
    {
        setStyleSheet(css.arg(css_colors["selected"][tile_color]).arg(css_colors["hover"][tile_color]));
    }

    void Tile::dyeValid()
    {
        setStyleSheet(css.arg(css_colors["valid"][tile_color]).arg(css_colors["hover"][tile_color]));
    }

    VirtualTile Tile::toVirtu()
    {
        return { coord, piece_name, piece_color };
    }
}  // namespace mmd