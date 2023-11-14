#ifndef VALIDATION_H
#define VALIDATION_H
#include <set>
#include <list>
#include <algorithm>
#include <utility>
#include <algorithm>
#include <functional>
#include "local_types.h"

class Board;
class Tile;
//struct scoord;
//using pint = std::pair<int, int>;
//using pintr = const std::pair<int, int>&;
using lambda = std::function<bool(scoord)>;
using func = bool (*)(scoord);

class Validation{

    Board& board;
    std::set<Tile*> valid_moves;
    void findValid(Tile* from);
    bool underAttack(scoord coord);
    bool runThrough(scoord coord, scoord add, lambda stop_cond, lambda borders_cond);
    bool onDiagonals(scoord coord, lambda stop_cond, lambda borders_cond);
    bool fastDiagonals(scoord coord, lambda stop_cond, lambda borders_cond);
    bool onPerp(scoord coord, lambda stop_cond, lambda borders_cond);
    bool fastPerp(scoord coord, lambda stop_cond, lambda borders_cond);
    void kingPotential(scoord coord, std::list<scoord>& coords);
    void knightPotential(scoord coord, std::list<scoord>& coords);
    bool notAlignKing(scoord coord, scoord king, scoord from);

public:
    Validation(Board* board) : board(*board) {};
 
    void showValid(Tile * from);
    void hideValid();
    bool isValid(Tile * move);
    bool empty();
    bool inCheck(Tile * king);
    bool inCheckmate(Tile * king);
    bool inStalemate(bool color);
};

#endif // VALIDATION_H