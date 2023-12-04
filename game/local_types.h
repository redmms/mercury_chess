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
    draw_by_agreement,
    draw_by_stalemate,
    user_wins,
    opponent_wins,
    user_resignation,
    opponent_resignation,
    user_out_of_time,
    opponent_out_of_time,
    draw_by_repetition, // FIX: need to handle
    draw_by_insufficient_material  // FIX: need to handle
};

enum tatus : int {
    check_to_user,
    check_to_opponent,
    users_piece_eaten,
    opponents_piece_eaten,
    draw_suggestion,
    castling,
    promotion,
    just_new_turn,
    invalid_move
};

struct scoord{
    int x = 0;
    int y = 0;
    
    bool operator != (const scoord right) const {
        return x != right.x || y != right.y;
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
