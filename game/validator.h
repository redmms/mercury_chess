#pragma once
#include "virtual_validator.h"
#include "tile.h"

class Board;
class Validator : public VirtualValidator {
protected:
    Board* board;

public:
    Validator(Board* mother_board = 0);

    Tile& theTile(scoord) override;
    bool theTurn() override;
    Tile& theWKing() override;
    Tile& theBKing() override;
    void moveVirtually(scoord, scoord, vove&) override;
    void revertVirtualMove(vove&) override;
    const std::vector<halfmove>& theStory() override;

    void showValid(scoord from);
    void hideValid();
};