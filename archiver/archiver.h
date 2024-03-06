#pragma once
#include "../app/local_types.h"
import finestream;

class Validator;
class VirtualValidator;
class VirtualBoard;
class Archiver : public QObject {
	Q_OBJECT

	const uint8_t archiver_version = 0;

public:
	Archiver(QObject* parent = 0);
	
	int writeGame(endnum end_type, const std::vector<bitmove>& history, std::string filename);
	inline int writeMove(bitmove move, fsm::ofinestream& ofs);
	int readGame(endnum& end_type, std::vector<halfmove>& history, std::string filename);
	inline int readMove(bitmove& bmove, fsm::ifinestream& ifs, VirtualBoard& board);
	bitmove toBitmove(halfmove hmove, Validator& valid);
	halfmove toHalfmove(bitmove bmove, VirtualValidator& valid);
};