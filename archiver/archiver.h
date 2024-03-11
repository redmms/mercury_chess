#pragma once
#include "../app/local_types.h"
import finestream;

class Validator;
class VirtualValidator;
class VirtualBoard;
class Archiver : public QObject {

    const uint8_t archiver_version = 0;

public:
    Archiver();
    
    int writeGame(endnum end_type, const std::vector<bitmove>& history, std::string filename);
    int writeMove(bitmove move, fsm::ofinestream& ofs);
    int readGame(endnum& end_type, std::vector<halfmove>& history, std::string filename);
    int readMove(bitmove& bmove, fsm::ifinestream& ifs, VirtualBoard& board);
    static bitmove toBitmove(halfmove hmove, Validator* valid);
    static bitremedy toPieceIdx(scoord from, Validator* valid);
    static bitremedy toMoveIdx(scoord to, Validator* valid);
    static halfmove toHalfmove(bitmove bmove, VirtualValidator* valid);
};
