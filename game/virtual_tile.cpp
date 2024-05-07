#include "virtual_tile.h"
#include "virtual_board.h"
using namespace std;

namespace mmd
{
    VirtualTile::VirtualTile(scoord tile_coord_, char piece_name_, bool piece_color_, VirtualBoard* mother_board_) :
        coord(tile_coord_),
        piece_name(piece_name_),
        piece_color(piece_color_),
        tile_color((coord.x + coord.y) % 2),
        board(mother_board_)
    {}

    scoord VirtualTile::Coord() const
    {
        return coord;
    }

    char VirtualTile::PieceName() const
    {
        return piece_name;
    }

    bool VirtualTile::PieceColor() const
    {
        return piece_color;
    }

    bool VirtualTile::TileColor() const
    {
        return tile_color;
    }

    void VirtualTile::setPiece(char name, bool color, bool virtually)
    {
        piece_name = name;
        piece_color = color;
        if (name == 'K' && board) {
            if (color) board->setWKing(coord);
            else board->setBKing(coord);
        }
    }

    bool VirtualTile::operator==(VirtualTile& r)
    {
        return coord == r.coord && piece_name == r.piece_name && piece_color == r.piece_color;
    }

    bool VirtualTile::operator!=(VirtualTile& r)
    {
        return !(*this == r);
    }

    QString halfmoveToString(halfmove hmove)
    {
        VirtualTile vf = hmove.move.first;
        VirtualTile vt = hmove.move.second;
        scoord f = vf.Coord();
        scoord t = vt.Coord();
        char piece = vf.PieceName();
        char promo = hmove.promo;

        QString out;
        if (hmove.castling) {
            out = "O-O";
        }
        else {
            if (piece != 'P')
                out += piece;
            out += coordToString(f) + coordToString(t);
            if (promo != 'e')
                out += "=" + QString(promo);
        }
        return out;
    }
}  // namespace mmd