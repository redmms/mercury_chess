#ifndef VALIDATION_H
#define VALIDATION_H
#include <set>
#include <list>
#include <functional>
#include "local_types.h"
class Board;
class Tile;

class Validation
{
    Board& board; 
    std::set<Tile*> valid_moves;
    bool check = false;
    bool has_moved[6]{ false };
    scoord rooks_kings[6]{ {0,0}, {4, 0}, {7, 0}, {0, 7}, {4, 7}, {7, 7} };
    scoord castling_destination[6]{ {2, 0}, {-1, -1}, {6, 0}, {2, 7}, {-1, -1}, {6, 7} };
    std::list<scoord> should_be_free[6]{
        {{3, 0}, {2, 0}, {1, 0}},
        {},
        {{5, 0}, {6, 0}},
        {{3, 7}, {2, 7}, {1, 7}},
        {},
        {{5, 7}, {6, 7}}
    };
    std::list<scoord> should_be_safe[6]{
        {{4, 0}, {3, 0}, {2, 0}},
        {},
        {{4, 0}, {5, 0}, {6, 0}},
        {{4, 7}, {3, 7}, {2, 7}},
        {},
        {{4, 7}, {5, 7}, {6, 7}}
    };
    std::list<scoord> perp_dir{{ 0,  1 }, { 0,  -1 }, { 1,  0 },  { -1, 0 }};
    std::list<scoord> diag_dir{{ 1,  1 }, { 1, -1 },  { -1,  1 }, { -1, -1 }};
    
    std::function<Tile*(scoord)> theTile;
    std::function<bool(scoord)> inBoard;
    std::function<bool(scoord)> occupied;
    std::function<bool(scoord)> differentColor;
    std::function<char(scoord)> pieceName;
    std::function<void(scoord)> addValid;

    void kingPotential(scoord coord, std::list<scoord>& coords);
    void knightPotential(scoord coord, std::list<scoord>& coords);
    bool underAttack(scoord coord);
    void findValid(Tile* from);

public:
    Validation(Board* mother_board);
 
    void showValid(Tile * from);
    void hideValid();
    bool isValid(Tile * move);
    bool empty();
    bool inCheck(bool color);
    bool inCheckmate(bool color);
    bool inStalemate(bool color);
    bool canCastle(Tile* from, Tile* to, Tile** rook);
    bool canPass(Tile* from, Tile* to);
    bool canPromote(Tile* pawn, Tile* destination);
    void reactOnMove(Tile* from, Tile* to);
};

#endif // VALIDATION_H
