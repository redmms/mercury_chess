#pragma once
#include "virtual_validator.h"
#include "tile.h"

namespace mmd
{
    class Board;
    class Validator : public VirtualValidator 
    {
        Board* board;

    public:
        Validator(Board* mother_board);

        Tile* theTile(scoord) override;
        void showValid(scoord from);
        void hideValid();
        inline bool inStalemate() override;
    };
}  // namespace mmd