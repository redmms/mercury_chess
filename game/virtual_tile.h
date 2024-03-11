#pragma once
#include "../app/local_types.h"

class VirtualBoard;
class VirtualTile {
public:
    VirtualBoard* board;
    scoord coord;
    char piece_name;
    bool piece_color;
    bool tile_color;
    
    VirtualTile(scoord tile_coord_ = {}, char piece_name_ = 'e', bool piece_color_ = false, VirtualBoard* parent_ = 0);

    virtual void setPiece(char elem, bool color, bool virtually = false);
};

struct halfmove {
    vove move;
    char promo = 'e';
    bool castling = false;
    bool pass = false;
    bool turn = false;
};