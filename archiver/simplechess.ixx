module;
#include "../app/local_types.hpp"
#include "../game/validator.h"
#include "../game/board.h"
#include "../game/tile.h"
#include <QPointer>
#include <QString>
#include <QSettings>
#include <windows.h>
#include <QThread>
#include <QApplication>
#include <ostream>
#include <QDebug>

export module simplechess;
import bitremedy;
import finestream;
import <iostream>;
import <bitset>;
import <vector>;
import <string>;
import <cmath>;
import <map>;
import <stdexcept>;
import <set>;
import <iterator>;
using namespace std;
constexpr auto CHB = CHAR_BIT;

using uchar = unsigned char;
//constexpr int CHB = CHAR_BIT;
constexpr int CHB1 = CHAR_BIT - 1;
//
////import <utility>;
//std::ostream& operator << (std::ostream& OUT, const bitremedy& BR) {
//	uchar MASK{1};
//	if (BR.MOVED_LEFT) {
//		MASK <<= CHB1;
//	}
//	else {
//		MASK <<= BR.BITSN - 1;
//	}
//	for (int I = 0; I < BR.BITSN; MASK >>= 1, I++) {
//		OUT << (BR.UCBYTE & MASK);
//	}
//	return OUT;
//}

//std::ostream& operator << (std::ostream& OUT, const bitremedy& BR) {
//	uchar MASK{ 1 };
//	if (BR.MOVED_LEFT) {
//		MASK <<= CHB1;
//	}
//	else {
//		MASK <<= BR.BITSN - 1;
//	}
//	for (int I = 0; I < BR.BITSN; MASK >>= 1, I++) {
//		OUT << (BR.UCBYTE & MASK ? '1' : '0');
//	}
//	return OUT;
//}


//std::ostream& operator << (std::ostream& OUT, const bitremedy& BR) {
//	uchar MASK{ 1 };
//	if (BR.MOVED_LEFT) {
//		MASK <<= CHB1;
//	}
//	else {
//		MASK <<= BR.BITSN - 1;
//	}
//	string res{""};
//	for (int I = 0; I < BR.BITSN; MASK >>= 1, I++) {
//		res.push_back(BR.UCBYTE & MASK ? '1' : '0');
//	}
//	OUT.write(res.c_str(), BR.BITSN);
//	return OUT;
//}

//void write(ostream& OUT, const bitremedy& BR) {
//	uchar MASK{1};
//	if (BR.MOVED_LEFT) {
//		MASK <<= CHB1;
//	}
//	else {
//		MASK <<= BR.BITSN - 1;
//	}
//	for (int I = 0; I < BR.BITSN; MASK >>= 1, I++) {
//		OUT.put(BR.UCBYTE & MASK ? '1' : '0');
//	}
//}


QDebug operator << (QDebug OUT, const bitremedy& BR) {
	uchar MASK{ 1 };
	if (BR.MOVED_LEFT) {
		MASK <<= CHB1;
	}
	else {
		MASK <<= BR.BITSN - 1;
	}
	string res{""};

	for (int I = 0; I < BR.BITSN; MASK >>= 1, I++) {
		res.push_back(BR.UCBYTE & MASK ? '1' : '0');
	}
	qDebug() << res.c_str();
	return qDebug();
}

void stdwrite(const bitremedy& BR) {
	uchar MASK{ 1 };
	if (BR.MOVED_LEFT) {
		MASK <<= CHB1;
	}
	else {
		MASK <<= BR.BITSN - 1;
	}
	string res{ "" };
	for (int I = 0; I < BR.BITSN; MASK >>= 1, I++) {
		res.push_back(BR.UCBYTE & MASK ? '1' : '0');
	}
	cout << res.c_str();
}

export class Archiver {
	const uint8_t archiver_version = 0;
	Board* board;
	Validator* valid;
	QPointer<Tile> from;
	QPointer<Tile> to;
	QSettings& settings;
	QApplication& app;
	fsm::ofinestream ofs;
	fsm::ifinestream ifs;
	uint8_t moves_num;
public:
	Archiver(QSettings& settings_, QApplication* app_) : 
		settings(settings_),
		app(*app_)
	{}

	int writeGame(Board* board_, std::string filename) 
	{	
		board = board_;
		valid = board->valid.data();
		endnum end_type = board->end_type;
		const auto& history = board->bistory;
		try {
			fsm::ofinestream pofs(filename);
		}
		catch (const exception& e) {
			//cerr << e.what() << endl;
			return 1;
		}
		ofs.open(filename);
		if (history.empty()) {
			//cerr << "You are trying to write an empty game" << endl;
			return 2;
		}
		string opp_name = settings.value("opp_name").toString().toStdString();
		uint8_t char_num = opp_name.size();
		bool user_side = settings.value("match_side").toBool();
		uint8_t moves_num = history.size();
		bitremedy end(end_type, 4, false);
		ofs << archiver_version
			<< char_num
			<< opp_name
			<< user_side
			<< moves_num
			<< end;
		cout << " side: " << user_side
				<< " moves: " << bitset<CHB>(moves_num)
				<< " end: " << end.toStr();
		for (auto bmove : history) {		
			//bitmove bmove = toBitmove(hmove);
			writeMove(bmove, ofs);
		}
		//cout << "Ending bits number in the end of file is " << ofs.ExtraZerosN() << endl;
		return 0;
	}
	int readHeader(std::string filename) {
		//auto& history = board->history;
		try {
			fsm::ifinestream pifs(filename);
		}
		catch (const exception& e) {
			//cerr << e.what() << endl;
			return 1;
		}
		ifs.open(filename);
		//history.clear();

		//settings.setValue("game_regime", "history");
		uint8_t version;
		ifs >> version;
		//cout << "DEBUG: version number is " << int(version) << endl;
		if (version != archiver_version) {
			//cerr << "You are using inappropriate archiver version for the file" << endl;
			return 2;
		}

		uint8_t char_num;
		ifs >> char_num;
		//cout << "DEBUG: opp_name size is " << int(char_num) << endl;

		string opp_name(char_num, ' ');
		ifs >> opp_name;
		//cout << "DEBUG: opp_name is " << opp_name << endl;
		settings.setValue("opp_name", QString(opp_name.c_str()));

		bool user_side;
		ifs >> user_side;
		//cout << "DEBUG: user side is " << (user_side ? "white" : "black") << endl;
		settings.setValue("match_side", user_side);

		//uint8_t moves_num;
		ifs >> moves_num;
		//cout << "DEBUG: moves number is " << int(moves_num) << endl;


		cout << " side: " << user_side
			<< " moves: " << bitset<CHB>(moves_num);
		return 0;
	}
	int readMoves(Board* board_) {
		board = board_;
		valid = board->valid.data();
		bitremedy end_type{ 0, 4, false };
		ifs >> end_type;
		//cout << "DEBUG: end type number is " << int(end_type) << endl;
		board->end_type = endnum(int(end_type));
		cout << " end: " << end_type.toStr();

		valid->inStalemate(true);
		for (int i = 0; i < moves_num; i++) {
			//cout << "DEBUG: move " << i + 1 << ":" << endl;
			bitmove bmove;
			int err = readMove(bmove, ifs);
			if (err) {
				return err;
			}
			//halfmove hmove = toHalfmove(bmove);
			//history.push_back(hmove);
		}
		return 0;
	}
	//int readGame(std::string filename) 
	//{



	//}
	inline int writeMove(bitmove move, fsm::ofinestream& ofs) {
	// TODO: should go as an extern operator
	// for ofinestream
		ofs << move.piece
			<< move.move;
		cout << " piece: " << move.piece.toStr()
				<< " move: " << move.move.toStr();
		if (move.promo != no_promotion) {
			ofs << bitset<2>(int(move.promo));
			cout << " promo: " << bitset<2>(int(move.promo));
		}
		return 0;
	}
	inline int readMove(bitmove& bmove, fsm::ifinestream& ifs) {
		bmove.piece.BITSN = minBits(valid->movable_pieces.size() - 1);
		ifs >> bmove.piece;
		//cout << "DEBUG: piece BITSN is " << bmove.piece.BITSN << endl;
		//cout << "DEBUG: piece idx is " << int(bmove.piece) << endl;
		////cout << "DEBUG: piece pattern is " << hex << int(bmove.piece) << endl;
		int pidx = bmove.piece;
		if (pidx >= valid->movable_pieces.size()) {
			//cerr << "WARNING: piece idx is bigger than pieces to move available" << endl;
			return 1;
		}
		from = *next(valid->movable_pieces.begin(), int(bmove.piece)); 

		valid->findValid(from);
		bmove.move.BITSN = minBits(valid->valid_moves.size() - 1);
		ifs >> bmove.move;
		//cout << "DEBUG: move BITSN is " << bmove.move.BITSN << endl;
		//cout << "DEBUG: move idx is " << int(bmove.move) << endl;
		////cout << "DEBUG: move pattern is " << hex << int(bmove.move) << endl;
		int midx = bmove.move;
		if (midx >= valid->valid_moves.size()) {
			//cerr << "WARNING: move idx is bigger than valid moves available" << endl;
			return 2;
		}
		to = *next(valid->valid_moves.begin(), int(bmove.move));
		cout << " piece: " << bmove.piece.toStr()
			<< " move: " << bmove.move.toStr();
		// FIX: would be perfect to change to QPointer<>
		if (valid->canPromote(from, to)) {
			bitset<2> promo;
			ifs >> promo;
			bmove.promo = promo_ty(promo.to_ulong());
			board->last_promotion = char_by_promo[bmove.promo];
			//cout << "DEBUG: promotion type is " << board->last_promotion << endl;
			cout << " promo: " << promo;
		}
		board->halfMove(from, to);
		app.processEvents();
		QThread::sleep(1);
		return 0;
	}

	template<typename T>
	int minBits(T number) {
		if (!number) {
			//cerr << "WARNING: minBits input was 0, output is 0 (0 bit), this case should be user defined" << endl;
		}
		return ceil(log2(number + 1));
	}
	bitmove toBitmove(halfmove hmove) {
		bitmove bmove;
		QPointer<Tile> from = hmove.move.first.tile;
		QPointer<Tile> to = hmove.move.second.tile;
		unsigned char piece_idx = distance(valid->movable_pieces.begin(), valid->movable_pieces.find(from));
		unsigned char move_idx = distance(valid->valid_moves.begin(), valid->valid_moves.find(to));
		bmove.piece = bitremedy(piece_idx, minBits(valid->movable_pieces.size()), false);
		bmove.move = bitremedy(move_idx, minBits(valid->valid_moves.size()), false);
		bmove.promo = promo_by_char[hmove.promo];
		return bmove;
	}
	halfmove toHalfmove(bitmove bmove) {
		//from = *next(valid->movable_pieces.begin(), bmove.piece);
		//to = *next(valid->valid_moves.begin(), bmove.move);
		halfmove hmove;
		hmove.move = { from->toVirtu(), to->toVirtu() };
		hmove.promo = char_by_promo[bmove.promo];
		Tile* rook_stub;
		hmove.castling = valid->canCastle(from, to, rook_stub);
		hmove.pass = valid->canPass(from, to);
		hmove.turn = board->turn;
		return hmove;
	}
};


//for (int i = 0; i < moves_num; i++) { // TODO: should go as an extern operator
//	// for ofinestream
//	bitmove& move = board->bistory.story[i];
//	ifs >> move.piece
//		>> move.move;
//	if (move.promo != no_promotion) {
//		bitset<2> promo;
//		ifs >> promo;
//		move.promo = promo_ty(promo.to_ulong());
//	}
//}

//int ConvertMove(bitmove& bmove, halfmove& hmove, Validator& valid) {
//	Tile* from = *next(valid->movable_pieces.begin(), bmove.piece);
//	Tile* to = *next(valid->valid_moves.begin(), bmove.move);
//	hmove.move = { from->toVirtu(), to->toVirtu() };
//	hmove.promo = char_by_promo[bmove.promo];
//	Tile* rook_stub;
//	hmove.castling = valid->canCastle(from, to, rook_stub);
//	hmove.pass = valid->canPass(from, to);
//	hmove.turn = valid->board->turn;
//	return 0;
//}
