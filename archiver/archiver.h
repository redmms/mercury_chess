#pragma once
#include "../app/local_types.h"

namespace fsm {
    class ofinestream;
    class ifinestream;
}
class Validator;
class VirtualValidator;
class VirtualBoard;
class Archiver 
{
    const uint8_t archiver_version = 0;

public:
    Archiver();
    
    int writeGame(endnum end_type, const std::vector<bitmove>& history, QString filename);
    int writeMove(bitmove move, fsm::ofinestream& ofs);
    int readGame(endnum& end_type, std::vector<halfmove>& history, QString filename);
    int readMove(halfmove& hmove, fsm::ifinestream& ifs, VirtualBoard& board);
    static bitmove toBitmove(halfmove hmove, Validator* valid);
    static bitremedy toPieceIdx(scoord from, Validator* valid);
    static bitremedy toMoveIdx(scoord to, Validator* valid);
    static halfmove toHalfmove(bitmove bmove, VirtualValidator* valid);
};
