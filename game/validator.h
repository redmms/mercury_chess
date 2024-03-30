#pragma once
#include "virtual_validator.h"
#include "tile.h"

class Board;
class Validator : public VirtualValidator {
protected:
    Board* board;

public:
    Validator(Board* mother_board);

    Tile* theTile(scoord) override;
    void showValid(scoord from);
    void hideValid();
    inline bool inStalemate(bool color) override;
};