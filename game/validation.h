#ifndef VALIDATION_H
#define VALIDATION_H
#include <set>
#include <algorithm>
#include <utility>
#include <algorithm>
#include <functional>

class Board;
class Tile;
using pint = std::pair<int, int>;
using pintr = const std::pair<int, int>&;
using lambda = std::function<bool(pint)>;
using func = bool (*)(pint);

class Validation{

    Board& m_board;
    std::set<Tile*> m_valid_moves;
    void findValid(Tile* from);
    bool underAttack(pintr coord);
    bool runThrough(pint coord, pintr add, lambda stop_cond, lambda borders_cond);
    bool onDiagonals(pintr coord, lambda stop_cond, lambda borders_cond);
    bool fastDiagonals(pintr coord, lambda stop_cond, lambda borders_cond);
    bool onPerp(pintr coord, lambda stop_cond, lambda borders_cond);
    bool fastPerp(pintr coord, lambda stop_cond, lambda borders_cond);
    void kingPotential(pintr coord, std::set<pint>& coords);
    void knightPotential(pintr coord, std::set<pint>& coords);

public:
    Validation(Board* board) : m_board(*board) {};
 
    void showValid(Tile * from);
    void hideValid();
    bool isValid(Tile * move);
    bool empty();
    bool inCheck(Tile * king);
    bool inCheckmate(Tile * king);
    bool inStalemate(bool color);
};

#endif // VALIDATION_H