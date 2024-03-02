#pragma once
#include "archiver.h"
#include "../game/board.h"
#include <QDebug>
#include <sstream>
using namespace std;

//QDebug operator << (QDebug OUT, bitremedy BR) {
//	qDebug() << BR.toStr().c_str();
//	return qDebug();
//}

//stringstream sout;

Archiver::Archiver(QObject* parent) :
	QObject(parent)
{}

int Archiver::writeGame(Board* board_, std::string filename)
{
	board = board_;
	Validator& valid = board->valid;
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
	string opp_name = settings["opp_name"].toString().toStdString();
	uint8_t char_num = opp_name.size();
	bool user_side = settings["match_side"].toBool();
	uint8_t moves_num = history.size();
	bitremedy end(end_type, 4, false);
	ofs << archiver_version
		<< char_num
		<< opp_name
		<< user_side
		<< moves_num
		<< end;
	//cout << " side: " << user_side
	//	<< " moves: " << bitset<CHB>(moves_num)
	//	<< " end: " << end.toStr();
	for (auto bmove : history) {
		//bitmove bmove = toBitmove(hmove);
		writeMove(bmove, ofs);
	}
	//cout << "Ending bits number in the end of file is " << ofs.ExtraZerosN() << endl;
	return 0;
}

int Archiver::readHeader(std::string filename) {
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

	//settings[ ] = ("game_regime", "history");
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
	settings["opp_name"].setValue(QString(opp_name.c_str()));

	bool user_side;
	ifs >> user_side;
	//cout << "DEBUG: user side is " << (user_side ? "white" : "black") << endl;
	settings["match_side"].setValue(user_side);

	//uint8_t moves_num;
	ifs >> moves_num;
	//cout << "DEBUG: moves number is " << int(moves_num) << endl;

	//sout << " side: " << user_side
	//	<< " moves: " << bitset<CHB>(moves_num)
	//	<< endl;
	return 0;
}

int Archiver::readMoves(Board* board_) {
	board = board_;
	Validator& valid = board->valid;
	bitremedy end_type{ 0, 4, false };
	ifs >> end_type;
	//cout << "DEBUG: end type number is " << int(end_type) << endl;
	board->end_type = endnum(int(end_type));
	//cout << " end: " << end_type.toStr();

	valid.inStalemate(true);
	for (int i = 0; i < moves_num; i++) {
		//cout << "DEBUG: move " << i + 1 << ":" << endl;
		int order = i + 1;
		//if ( order % 2)
		//	sout << (order - 1) / 2 + 1 << "." << endl;
		bitmove bmove;
		int err = readMove(bmove, ifs);
		if (err) {
			//cout << sout.str();
			return err;
		}
		//halfmove hmove = toHalfmove(bmove);
		//history.push_back(hmove);
	}
	//cout << sout.str();
	return 0;
}

inline int Archiver::writeMove(bitmove move, fsm::ofinestream& ofs) {
	// TODO: should go as an extern operator
	// for ofinestream
	ofs << move.piece
		<< move.move;
	//cout << " piece: " << move.piece.toStr()
	//	<< " move: " << move.move.toStr()
	//	<< endl;
	if (move.promo != no_promotion) {
		ofs << bitset<2>(int(move.promo));
		//cout << " promo: " << bitset<2>(int(move.promo));
	}
	return 0;
}

inline int Archiver::readMove(bitmove& bmove, fsm::ifinestream& ifs) {
	Validator& valid = board->valid;
	bmove.piece.BITSN = fsm::MinBits(valid.movable_pieces.size() - 1);
	ifs >> bmove.piece;
	//cout << "DEBUG: piece BITSN is " << bmove.piece.BITSN << endl;
	//cout << "DEBUG: piece idx is " << int(bmove.piece) << endl;
	////cout << "DEBUG: piece pattern is " << hex << int(bmove.piece) << endl;
	int pidx = bmove.piece;
	if (pidx >= valid.movable_pieces.size()) {
		//cerr << "WARNING: piece idx is bigger than pieces to move available" << endl;
		return 1;
	}
	from = *next(valid.movable_pieces.begin(), int(bmove.piece));

	valid.findValid(from);
	bmove.move.BITSN = fsm::MinBits(valid.valid_moves.size() - 1);
	ifs >> bmove.move;
	//cout << "DEBUG: move BITSN is " << bmove.move.BITSN << endl;
	//cout << "DEBUG: move idx is " << int(bmove.move) << endl;
	////cout << "DEBUG: move pattern is " << hex << int(bmove.move) << endl;
	int midx = bmove.move;
	if (midx >= valid.valid_moves.size()) {
		//cerr << "WARNING: move idx is bigger than valid moves available" << endl;
		return 2;
	}
	to = *next(valid.valid_moves.begin(), int(bmove.move));
	//sout << " piece: " << bmove.piece.toStr()
	//	<< " move: " << bmove.move.toStr();
	// FIX: would be perfect to change to QPointer<>
	if (valid.canPromote(from, to)) {
		bitset<2> promo;
		ifs >> promo;
		bmove.promo = promo_ty(promo.to_ulong());
		board->last_promotion = char_by_promo[bmove.promo];
		//cout << "DEBUG: promotion type is " << board->last_promotion << endl;
		//sout << " promo: " << promo;
	}
	//sout << endl;
	board->halfMove(from, to);
	emit needUpdate();
	return 0;
}

bitmove Archiver::toBitmove(halfmove hmove) {
	bitmove bmove;
	Tile* from = hmove.move.first.tile;
	Tile* to = hmove.move.second.tile;
	Validator& valid = board->valid;
	unsigned char piece_idx = distance(valid.movable_pieces.begin(), valid.movable_pieces.find(from));
	unsigned char move_idx = distance(valid.valid_moves.begin(), valid.valid_moves.find(to));
	bmove.piece = bitremedy(piece_idx, fsm::MinBits(valid.movable_pieces.size()), false);
	bmove.move = bitremedy(move_idx, fsm::MinBits(valid.valid_moves.size()), false);
	bmove.promo = promo_by_char[hmove.promo];
	return bmove;
}

halfmove Archiver::toHalfmove(bitmove bmove) {
	//from = *next(valid.movable_pieces.begin(), bmove.piece);
	//to = *next(valid.valid_moves.begin(), bmove.move);
	halfmove hmove;
	hmove.move = { from->toVirtu(), to->toVirtu() };
	hmove.promo = char_by_promo[bmove.promo];
	Tile* rook_stub;
	Validator& valid = board->valid;
	hmove.castling = valid.canCastle(from, to, rook_stub);
	hmove.pass = valid.canPass(from, to);
	hmove.turn = board->turn;
	return hmove;
}