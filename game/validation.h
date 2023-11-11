#ifndef VALIDATION_H
#define VALIDATION_H
#include <set>
class Board;
class Tile;

class Validation{

    Board& m_board;
    std::set<Tile*> m_valid_moves;
    void findValid(Tile* from);

public:
    Validation(Board* board) : m_board(*board) {};
 
    void showValid(Tile * from);
    void hideValid();
    bool isValid(Tile * tile);
    bool empty();
};

#endif // VALIDATION_H