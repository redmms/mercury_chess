#ifndef VALIDATION_H
#define VALIDATION_H
#include "tile.h"

class Board;
class Validation{

public:

    Board& m_board;
    int
        m_valid_moves[28],
        m_valid_n = 0,
        m_wR = 0,
        m_wC = 0;

    Validation(Board* board) : m_board(*board)
    {};
                              
    int findValid(Tile* temp);
    bool validMove(Tile* from, Tile* to);
    void dyeOrange();
    void disOrange();
    int  check(Tile* temp);
};

#endif // VALIDATION_H
