#ifndef VALIDATION_H
#define VALIDATION_H
#include <set>
class Board;
class Tile;

class Validation{

    Board& m_board;
    void findValid(Tile* from);

public:
    Validation(Board* board) : m_board(*board) {};

    std::set<Tile*> m_valid_moves{ nullptr };
   
    void showPossible(Tile * from);
    void hidePossible();
};

#endif // VALIDATION_H