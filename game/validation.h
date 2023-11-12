#ifndef VALIDATION_H
#define VALIDATION_H
#include <set>
class Board;
class Tile;

class Validation{

    Board& m_board;
    std::set<Tile*> m_valid_moves;
    void findValid(Tile* from);
    void underAttack(Tile* tile);

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