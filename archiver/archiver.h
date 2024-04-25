#pragma once
#include "../app/local_types.h"

namespace fsm {
    class ofinestream;
    class ifinestream;
}

namespace mmd
{
    class Validator;
    class VirtualValidator;
    class VirtualBoard;
    class Archiver
    {
        const uint8_t archiver_version = 0;

    public:
        Archiver();

        int writeGame(endnum end_type, const std::vector<bitmove>& bistory, QString filename);
        int writeMove(bitmove bmove, fsm::ofinestream& ofs);
        int readGame(endnum& end_type, std::vector<bitmove>& bistory, std::vector<halfmove>& history, QString filename);
        int readMove(bitmove& bmove, halfmove& hmove, fsm::ifinestream& ifs, VirtualBoard& board);
        static bitmove toBitmove(halfmove hmove, Validator& valid);
        static bitremedy toPieceIdx(scoord from, Validator& valid);
        static bitremedy toMoveIdx(scoord to, Validator& valid);
        static halfmove toHalfmove(bitmove bmove, VirtualValidator& valid);
    };
}