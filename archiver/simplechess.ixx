module;
#include "../app/local_types.hpp"
#include "../game/validator.h"
#include "../game/board.h"
#include "../game/tile.h"
#include <QPointer>
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

//import <utility>;
export class Archiver {
	const uint8_t archiver_version = 0;
	Board& board;
	Validator& valid;
	QPointer<Tile> from;
	QPointer<Tile> to;
	Archiver(Board* board_) : 
		board(*board_),
		valid(*board.valid)
	{}
	int writeGame(std::string filename) {
		endnum end_type = board.end_type;
		const vector<halfmove>& history = board.history;
		try {
			fsm::ofinestream ofs(filename);
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
		uint8_t moves_num = history.size();
		bitremedy end(end_type, 4, false);
		ofs << archiver_version
			<< moves_num
			<< end;
		for (auto hmove : history) {		
			bitmove bmove = toBitmove(hmove);
			writeMove(bmove, ofs);
		}
		return 0;
	}
	int readGame(std::string filename) {
		auto& history = board.history;
		try {
			fsm::ifinestream ifs(filename);
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
			return 1;
		}
		fsm::ifinestream ifs(filename);
		history.clear();
		uint8_t version;
		ifs >> version;
		if (version != archiver_version) {
			cerr << "You are using inappropriate archiver version for the file" << endl;
			return 2;
		}
		uint8_t moves_num;
		ifs >> moves_num;
		bitremedy end_type{ 0, 4, false };
		ifs >> end_type;
		board.end_type = endnum(int(end_type));
		for (int i = 0; i < moves_num; i++) {
			bitmove bmove;
			readMove(bmove, ifs);
			halfmove hmove = toHalfmove(bmove);
			history.push_back(hmove);
		}
		return 0;
	}
	inline int writeMove(bitmove move, fsm::ofinestream& ofs) {
	// TODO: should go as an extern operator
	// for ofinestream
		ofs << move.piece
			<< move.move;
		if (move.promo != no_promotion) {
			ofs << bitset<2>(int(move.promo));
		}
		return 0;
	}
	inline int readMove(bitmove& bmove, fsm::ifinestream& ifs) {
		bmove.piece.BITSN = minBits(valid.movable_pieces.size());
		bmove.move.BITSN = minBits(valid.valid_moves.size());
		ifs >> bmove.piece
			>> bmove.move;
		from = *next(valid.movable_pieces.begin(), int(bmove.piece)); 
		to = *next(valid.valid_moves.begin(), int(bmove.move));
		// FIX: would be perfect to change to QPointer<>
		if (valid.canPromote(from, to)) {
			bitset<2> promo;
			ifs >> promo;
			bmove.promo = promo_ty(promo.to_ulong());
		}
		return 0;
	}
	template<typename T>
	int minBits(T number) {
		int bits_num = ceil(log2(number));
		return bits_num;
	}
	bitmove toBitmove(halfmove hmove) {
		bitmove bmove;
		QPointer<Tile> from = hmove.move.first.tile;
		QPointer<Tile> to = hmove.move.second.tile;
		int piece_idx = distance(valid.movable_pieces.begin(), valid.movable_pieces.find(from));
		int move_idx = distance(valid.valid_moves.begin(), valid.valid_moves.find(to));
		bmove.piece = bitremedy(piece_idx, minBits(piece_idx), false);
		bmove.move = bitremedy(move_idx, minBits(move_idx), false);
		bmove.promo = promo_by_char[hmove.promo];
		return bmove;
	}
	halfmove toHalfmove(bitmove bmove) {
		//from = *next(valid.movable_pieces.begin(), bmove.piece);
		//to = *next(valid.valid_moves.begin(), bmove.move);
		halfmove hmove;
		hmove.move = { from->toVirtu(), to->toVirtu() };
		hmove.promo = char_by_promo[bmove.promo];
		Tile* rook_stub;
		hmove.castling = valid.canCastle(from, to, &rook_stub);
		hmove.pass = valid.canPass(from, to);
		hmove.turn = board.turn;
		return hmove;
	}
};


//for (int i = 0; i < moves_num; i++) { // TODO: should go as an extern operator
//	// for ofinestream
//	bitmove& move = board.bistory.story[i];
//	ifs >> move.piece
//		>> move.move;
//	if (move.promo != no_promotion) {
//		bitset<2> promo;
//		ifs >> promo;
//		move.promo = promo_ty(promo.to_ulong());
//	}
//}

//int ConvertMove(bitmove& bmove, halfmove& hmove, Validator& valid) {
//	Tile* from = *next(valid.movable_pieces.begin(), bmove.piece);
//	Tile* to = *next(valid.valid_moves.begin(), bmove.move);
//	hmove.move = { from->toVirtu(), to->toVirtu() };
//	hmove.promo = char_by_promo[bmove.promo];
//	Tile* rook_stub;
//	hmove.castling = valid.canCastle(from, to, rook_stub);
//	hmove.pass = valid.canPass(from, to);
//	hmove.turn = valid.board.turn;
//	return 0;
//}
