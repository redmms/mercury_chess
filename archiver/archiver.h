#pragma once
#include "../app/local_types.hpp"
#include <QObject>
import finestream;

using namespace std;
using uchar = unsigned char;
constexpr int CHB = CHAR_BIT;
constexpr int CHB1 = CHAR_BIT - 1;

class Board;
class Validator;
class Tile;
class Archiver : public QObject {
	Q_OBJECT

	const uint8_t archiver_version = 0;
	Board* board;
	Validator* valid;
	QPointer<Tile> from;
	QPointer<Tile> to;
	fsm::ofinestream ofs;
	fsm::ifinestream ifs;
	uint8_t moves_num;

public:
	Archiver(QObject* parent);

	int writeGame(Board* board_, std::string filename);
	int readHeader(std::string filename);
	int readMoves(Board* board_);
	inline int writeMove(bitmove move, fsm::ofinestream& ofs);
	inline int readMove(bitmove& bmove, fsm::ifinestream& ifs);
	bitmove toBitmove(halfmove hmove);
	halfmove toHalfmove(bitmove bmove);

signals:
	void needUpdate();
};
