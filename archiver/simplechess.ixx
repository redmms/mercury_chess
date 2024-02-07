module;
#include "../app/local_types.hpp"
#include "../game/validator.h"
#include "../game/board.h"
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
using namespace std;
constexpr auto CHB = CHAR_BIT;

//import <utility>;
export class Archiver {
	const uint8_t archiver_version = 0;
	Archiver() {}
	int WriteGame(endnum end_type, const vector<bitmove>& bistory, std::string filename) {
		try {
			fsm::ofinestream ofs(filename);
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
			return 1;
		}
		fsm::ofinestream ofs(filename);
		if (bistory.empty()) {
			cerr << "You are trying to write an empty game" << endl;
			return 2;
		}
		bitremedy end(end_type, 4, false);
		uint8_t moves_num = bistory.size();
		ofs << archiver_version
			<< moves_num
			<< end;
		for (auto move : bistory) { // TODO: should go as an extern operator
			// for ofinestream
			ofs << move.piece
				<< move.move;
			if (move.promo != no_promotion) {
				ofs << bitset<2>(int(move.promo));
			}
		}
		return 0;
	}
	int ReadGame(Board& board, std::string filename) {
		Validator* valid = board.valid.data();
		try {
			fsm::ifinestream ifs(filename);
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
			return 1;
		}
		fsm::ifinestream ifs(filename);
		board.history.clear();
		board.bistory.clear();
		uint8_t version;
		ifs >> version;
		if (version != archiver_version) {
			cerr << "You use inappropriate archiver version for the file" << endl;
			return 2;
		}
		uint8_t moves_num;
		ifs >> moves_num;
		bitremedy end_type{ 0, 4, false };
		ifs >> end_type;
		board.end_type = endnum(int(end_type));
		for (int i = 0; i < moves_num; i++) { // TODO: should go as an extern operator
			// for ofinestream
			bitmove move;
			move.piece.BITSN = minBits(valid->movable_pieces.size());
			move.move.BITSN = minBits(valid->valid_moves.size());
			ifs >> move.piece
				>> move.move;

		//	Tile* from_tile = valid->theTile();
		//	bool can_castle = valid->canCastle();
			if (move.promo != no_promotion) {
				bitset<2> promo;
				ifs >> promo;
				move.promo = promo_ty(promo.to_ulong());
			}
			board.bistory.push_back(move);
		}

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
		return 0;
	}
	int WriteMove(bitmove move, fsm::ofinestream& ofs) {
		return 0;

	}
	int ReadMove(bitmove& move, fsm::ifinestream& ifs) {
		return 0;

	}
	int ConvertMove(bitmove& bmove, halfmove& hmove) {
		return 0;
	}
	template<typename T>
	int minBits(T number) {
		int bits_num = ceil(log2(number));
		return bits_num;
	}
};
