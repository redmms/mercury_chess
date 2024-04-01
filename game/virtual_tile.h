#pragma once
#include "../app/local_types.h"

class VirtualBoard;
class VirtualTile {
public:
    scoord coord;
    char piece_name;
    bool piece_color;
    bool tile_color;
    VirtualBoard* board;

    VirtualTile(scoord tile_coord_ = {}, char piece_name_ = 'e', bool piece_color_ = false, VirtualBoard* parent_ = 0);

    virtual void setPiece(char elem, bool color, bool virtually = false);

    bool operator == (VirtualTile& r) {
        return coord == r.coord && piece_name == r.piece_name && piece_color == r.piece_color /*&& tile_color == r.tile_color && board == r.board*/;
    }

    bool operator != (VirtualTile& r) {
        return coord != r.coord || piece_name != r.piece_name || piece_color != r.piece_color /*|| tile_color != r.tile_color || board != r.board*/;
    }
};

struct halfmove {
    vove move;
    char promo = 'e';
    bool castling = false;
    bool pass = false;
    bool turn = false;
    bool check = false;
    unsigned char moved = 0;
};

inline QString halfmoveToString(halfmove hmove)
{
    VirtualTile vf = hmove.move.first;
    VirtualTile vt = hmove.move.second;
    scoord f = vf.coord;
    scoord t = vt.coord;
    char piece = vf.piece_name;
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