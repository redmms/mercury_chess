#pragma once
#include "../app/local_types.h"

namespace mmd
{
    class VirtualBoard;
    class VirtualTile 
    {
    protected:
        scoord coord;
        char piece_name;
        bool piece_color;
        bool tile_color;
        VirtualBoard* board;

    public:
        VirtualTile(scoord tile_coord_ = {}, char piece_name_ = 'e', bool piece_color_ = false, VirtualBoard* parent_ = 0);
        virtual ~VirtualTile() {}

        scoord Coord() const;
        char PieceName() const;
        bool PieceColor() const;
        bool TileColor() const;
        virtual void setPiece(char elem, bool color, bool virtually = false);

        bool operator == (VirtualTile& r);
        bool operator != (VirtualTile& r);
    };

    struct halfmove {
        vove move;
        char promo = 'e';
        bool castling = false;
        bool pass = false;
        bool turn = false;
        bool check = false;
        unsigned char moved = 0;
        unsigned char no_change_n = 0;
    };

    QString halfmoveToString(halfmove hmove);
}  // namespace mmd