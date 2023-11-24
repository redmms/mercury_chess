#pragma once
#include <utility>
#ifndef pove
#define pove std::pair<virtu, virtu>
#define povec const pove& 
#endif // !pove && !povec // WARNING: when you include this file,
// place #endif to the end of your file
// or use #undef after #include or include this file after other files
// using defines for pove, povec and coorder
#ifndef coorder
#define coorder const scoord& 
#endif // !coorder

enum endnum : int {
    draw,
    white_wins,
    black_wins,
    white_resignation,
    black_resignation,
    stalemate,
    repetition,
    insufficient_material
};

enum tatus : int {
    check,
    new_turn,
    eaten_by_opp,
    eaten_by_user,
    castling,
    promotion,
    invalid_move
};

struct scoord{
    int x = 0;
    int y = 0;
    
    bool operator != (const scoord right) const {
        return x != right.x && y != right.y;
    }
    bool operator == (const scoord right) const {
        return x == right.x && y == right.y;
    }
};

//bool operator==(const scoord& left, const scoord& right) {
//    return left.x == right.x && left.y == right.y;
//}
//bool operator != (const scoord& left, const scoord& right) {
//    return left.x != right.x && left.y != right.y;
//}

class Tile;
struct virtu{
    Tile* tile;
    bool color;
    char name;
};
