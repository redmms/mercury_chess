#pragma once
#include "archiver.h"
#include "../game/virtual_board.h"
#include "../game/virtual_validator.h"
#include "../game/validator.h"
//#include <QDebug>
//#include <sstream>
using namespace std;

//stringstream sout;

Archiver::Archiver(QObject* parent) :
	QObject(parent)
{}

int Archiver::writeGame(endnum end_type, const std::vector<bitmove>& history, std::string filename)
{
	try {
		fsm::ofinestream pofs(filename);
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
	fsm::ofinestream ofs(filename);
	if (history.empty()) {
		cerr << "You are trying to write an empty game" << endl;
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
	//sout << " side: " << user_side
	//	   << " moves: " << bitset<8>(moves_num)
	//     << " end: " << end.toStr() << endl;
	for (auto bmove : history) {
		writeMove(bmove, ofs);
	}
	//cout << sout.str();
	//cout << "Ending bits number in the end of file is " << ofs.ExtraZerosN() << endl;
	return 0;
}

int Archiver::writeMove(bitmove move, fsm::ofinestream& ofs) {
	ofs << move.piece
		<< move.move;
	//sout << " piece: " << move.piece.toStr() << endl
	//	   << " move: " << move.move.toStr() << endl;
	if (move.promo != no_promotion) {
		ofs << bitset<2>(int(move.promo));
		//sout << " promo: " << bitset<2>(int(move.promo)) << endl;
	}
	return 0;
}

int Archiver::readGame(endnum& end_type, std::vector<halfmove>& history, std::string filename)
{
	try {
		fsm::ifinestream pifs(filename);
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
	fsm::ifinestream ifs(filename);

	uint8_t version;
	ifs >> version;
	if (version != archiver_version) {
		cerr << "You are using an inappropriate archiver version for this file" << endl;
		return 2;
	}

	uint8_t char_num;
	ifs >> char_num;

	string opp_name(char_num, ' ');
	ifs >> opp_name;
	settings["opp_name"].setValue(QString(opp_name.c_str()));

	bool user_side;
	ifs >> user_side;
	settings["match_side"].setValue(user_side);

	uint8_t moves_num;
	ifs >> moves_num;

	bitremedy end{ 0, 4, false };
	ifs >> end;
	end_type = endnum(int(end_type));
	
	//sout << " side: " << user_side
	//     << " moves: " << bitset<8>(moves_num)
	//     << " end: " << end_type.toStr();

	VirtualBoard board(this);
	VirtualValidator& valid = board.valid;
	valid.inStalemate(true);
	for (int i = 0; i < moves_num; i++) {
		//int order = i + 1;
		//if ( order % 2)
		//	sout << (order - 1) / 2 + 1 << "." << endl;
		bitmove bmove;
		int err = readMove(bmove, ifs, board);
		if (err) {
			//cout << sout.str();
			return err;
		}
		halfmove hmove = toHalfmove(bmove, valid);
		history.push_back(hmove);
		valid.valid_moves.clear();
	}
	//cout << sout.str();
	return 0;
}

inline int Archiver::readMove(bitmove& bmove, fsm::ifinestream& ifs, VirtualBoard& board) 
{
	VirtualValidator& valid = board.valid;
	bmove.piece.BITSN = fsm::MinBits(valid.movable_pieces.size() - 1);
	ifs >> bmove.piece;
	if (bmove.piece >= valid.movable_pieces.size()) {
		cerr << "WARNING: piece idx is bigger than movable pieces available" << endl;
		return 1;
	}
	scoord from = *next(valid.movable_pieces.begin(), int(bmove.piece));

	valid.findValid(from);
	bmove.move.BITSN = fsm::MinBits(valid.valid_moves.size() - 1);
	ifs >> bmove.move;
	if (bmove.move >= valid.valid_moves.size()) {
		cerr << "WARNING: move idx is bigger than valid moves available" << endl;
		return 2;
	}
	scoord to = *next(valid.valid_moves.begin(), int(bmove.move));
	//sout << " piece: " << bmove.piece.toStr()
	//	<< " move: " << bmove.move.toStr();

	if (valid.canPromote(from, to)) {
		bitset<2> promo;
		ifs >> promo;
		bmove.promo = promnum(promo.to_ulong());
		//sout << " promo: " << promo;
	}
	//sout << endl;
	board.halfMove(from, to, char_by_promo[bmove.promo]);
	return 0;
}

bitmove Archiver::toBitmove(halfmove hmove, Validator& valid) {
	bitmove bmove;
	scoord from = hmove.move.first.coord;
	scoord to = hmove.move.second.coord;
	unsigned char piece_idx = distance(valid.movable_pieces.begin(), valid.movable_pieces.find(from));
	unsigned char move_idx = distance(valid.valid_moves.begin(), valid.valid_moves.find(to));
	bmove.piece = bitremedy(piece_idx, fsm::MinBits(valid.movable_pieces.size() - 1), false);
	bmove.move = bitremedy(move_idx, fsm::MinBits(valid.valid_moves.size() - 1), false);
	bmove.promo = promo_by_char[hmove.promo];
	return bmove;
}

halfmove Archiver::toHalfmove(bitmove bmove, VirtualValidator& valid) {
	scoord from = *next(valid.movable_pieces.begin(), int(bmove.piece));
	scoord to = *next(valid.valid_moves.begin(), int(bmove.move)); // FIX: will these Validator members be actual and valid at the moment?
	halfmove hmove;
	hmove.move = { valid.theTile(from), valid.theTile(to) };
	hmove.promo = char_by_promo[bmove.promo];
	scoord rook_stub;
	hmove.castling = valid.canCastle(from, to, rook_stub);
	hmove.pass = valid.canPass(from, to);
	hmove.turn = valid.theTurn();
	return hmove;
}