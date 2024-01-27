module;
#include "C:\Qt_projects\multicolor_chess\src\game\tile.h"
#include "C:\Qt_projects\multicolor_chess\src\game\local_types.h"
export module bitchess;
import bitremedy;
import finestream;
import <iostream>;
import <bitset>;
import <vector>;
import <string>;
import <cmath>;
import <map>;
using namespace std;

enum piece_ty {
	knight,
	bishop,
	castle,
	queen,
	pawn,
	king
};
string piece_names[]{
	"knight",
	"bishop",
	"castle",
	"queen",
	"pawn",
	"king"
};
int move_sizes[6]{ // do we need this array?
	3,  // knight
	4,  // bishop
	4,  // castle
	5,  // queen
	2,  // pawn
	3   // king
};
enum promo_ty {
	to_knight,
	to_bishop,
	to_castle,
	to_queen,
	no_promotion
};
//N, B, R, Q
map<char, promo_ty> promoByChar{
	{'N', to_knight},
	{'B', to_bishop},
	{'R', to_castle},
	{'Q', to_queen},
	{'e', no_promotion}
};
//enum extra_ty {
//	end_by_board_situation,
//	technical_end,
//	draw,
//	white_resignation,
//	black_resignation,
//	classical_start,
//	extraordinary_start,
//	usual_move
//};
piece_ty black_pieces_by_idx[16]{  // do we need this array? Yes we do, it shows what idx a piece_typein my notation has
	castle,  // from 0 board idx
	knight,
	bishop,
	queen,
	king,
	bishop,
	knight,
	castle,

	pawn,
	pawn,
	pawn,
	pawn,
	pawn,
	pawn,
	pawn,
	pawn  // first two lines of board
	// c, kn, b, q, k, b, kn, c
};
piece_ty white_pieces_by_idx[16]{  // do we need this array? Yes we do, it shows what idx a piece_typein my notation has
	castle,  // from 0 board idx
	knight,
	bishop,
	queen,
	king,
	bishop,
	knight,
	castle,

	pawn,
	pawn,
	pawn,
	pawn,
	pawn,
	pawn,
	pawn,
	pawn  // first two lines of board
	// c, kn, b, q, k, b, kn, c
};
pair<int, int> white_pieces_coords[16]{
	{0, 0},
	{1, 0},
	{2, 0},
	{3, 0},
	{4, 0},
	{5, 0},
	{6, 0},
	{7, 0},

	{0, 1},
	{1, 1},
	{2, 1},
	{3, 1},
	{4, 1},
	{5, 1},
	{6, 1},
	{7, 1}
}; 
pair<int, int> black_pieces_coords[16]{
	{0, 7},
	{1, 7},
	{2, 7},
	{3, 7},
	{4, 7},
	{5, 7},
	{6, 7},
	{7, 7},

	{0, 6},
	{1, 6},
	{2, 6},
	{3, 6},
	{4, 6},
	{5, 6},
	{6, 6},
	{7, 6}
};
int8_t idxs_on_board[8][8]{ -1 };

export class Move {
private:
	bitset <4> 
		piece_idx;
	bitremedy 
		move{ 0, 0, 0 };
	inline static bool 
		white_turn = true;
	int 
		iBytes_num_bytes = 1;
	const uint8_t 
		decoder_version = 0;

public:
	Move(size_t max_game_length) {
	// game length is counted in halfmoves here. 256 length is for game of 128 full
	// moves. To make iBytes_num_bytes == 1 it should be around 220 or less.
	// Otherwise it will use 2 bytes throughout the file to write down byte sizes 
	// of games
		size_t bytes_num = ceil( (long double)(max_game_length * 9) / CHAR_BIT );
		// the number of bytes to write the longest game in the file
		// FIX: if it's a very long game of bots, for example, it should use 
		// long arithmancy
		long double bytes_num_bits = ceil(log2(bytes_num)); 
		// the number of bits to write
		// the byte size before of the longest game in the file
		// (written in the beggining of the file)
		iBytes_num_bytes = ceil(bytes_num_bits / CHAR_BIT);
		for (int i = 0; i < 16; i++) {
			const pair<int, int>& coord = white_pieces_coords[i];
			idxs_on_board[coord.first][coord.second] = i;
		}
		for (int i = 0; i < 16; i++) {
			const pair<int, int>& coord = black_pieces_coords[i];
			idxs_on_board[coord.first][coord.second] = i + 16;
		}
	};
	void write_game(endnum end_type, std::vector<halfmove> history, fsm::ofinestream& ofs){
		bitremedy bytes_num_bytes{ iBytes_num_bytes, 4, false};
		bitremedy moves_num_bytes{ 0, 4, false};
		size_t moves_num = history.size();
		if (moves_num > UCHAR_MAX){
			cerr << "ERROR: this data compressor version doesn't allow to save such big games";
			return;
		}
		long double moves_num_bits = ceil(log2(moves_num));
		size_t iMoves_num_bytes = ceil(moves_num_bits / CHAR_BIT);
		moves_num_bytes.UCBYTE = (unsigned char) iMoves_num_bytes;
		vector<bool> vbMoves_num(iMoves_num_bytes * CHAR_BIT);
		fsm::ToSizedVector(moves_num, vbMoves_num);
		vector<bool> vbBytes_num(iBytes_num_bytes * CHAR_BIT);
		fsm::ToSizedVector(char(0), vbBytes_num);
		int end_min_bits = ceil(log2((int)endnum::ENDNUM_MAX));
		// FIX: end_min_bits should be a constant class field and be equal for 
		// every game notation
		bitremedy brEnd_type{end_type, end_min_bits, false}; 
		ofs << decoder_version
			<< bytes_num_bytes
			<< moves_num_bytes
			<< vbBytes_num
			<< vbMoves_num
			<< brEnd_type;
			// FIX: the stream should return back here later and rewrite bytes_num_bytes;
			// thus I need to implement it in finestream
			// FIX: firstly; using this stream we should create an array (need to
			// implement << and >> for arrays and other containers)
			// FIX: bytes_num_bytes and moves_num_bytes should be fields,
			// it's better to know the longest game before constructing an object
			// of this class, and pass it as a parameter to constructor
			// the constructor should calculate an optimal bytes_num_bytes
			// size. Usually it shouldn't be bigger than moves_num_bytes
			// but if moves can need bigger bytes_num_bytes it should take the
			// biggest one sure it will be enough. 9 is the biggest number of 
			// bits per halfmove. Thus it should calculate ceil(log2(history.size() * 9))
		for (auto move : history){
			//struct halfmove {
			//	pove move;
			//	char promo = 'e';
			//	bool castling = false;
			//	bool pass = false;
			//	bool turn = false;
			//};
			pair <int, int> from_coord = move.move.first.tile->coord;
			pair <int, int> to_coord = move.move.second.tile->coord;
			promo_ty promo = promoByChar[move.promo];
			//extra_ty extra_type = extra_ty::usual_move;
			write_move(from_coord, to_coord, promo, ofs);
		}
		ofs.Flush();
	}
	void read_game(endnum& end_type, std::vector<halfmove>& history);
	void read_move(
		pair <int, int>& from_coord, 
		pair<int, int>& to_coord, 
		promo_ty & promo,
		fsm::ifinestream & ifs) 
	{
		//first go known parameters, than goes place to write result
		promo = promo_ty::no_promotion;

		ifs >> piece_idx;
		int	iPiece_idx = int(piece_idx.to_ulong());
		from_coord = white_turn ? white_pieces_coords[iPiece_idx] : black_pieces_coords[iPiece_idx];

		piece_ty
			piece_type = white_turn ? white_pieces_by_idx[iPiece_idx] : black_pieces_by_idx[iPiece_idx];
		int 
			x, y, n, group,
			x0 = from_coord.first,
			y0 = from_coord.second,
			size = move_sizes[piece_type];

		//if (white_turn && piece_type == king)
		//	size++;

		move.BITSN = size;
		ifs >> move; // check if it's left aligned (should be right aligned)
		n = move.UCBYTE;

		switch (piece_type) {
		case pawn:
			if (white_turn) {
				x = x0 - 1 + n % 3 + n / 3;
				y = y0 + 1 + n / 3;
				if (y == 7){
					bitset<2> bs2{0};
					ifs >> bs2;
					promo = (promo_ty)bs2.to_ulong();
				}
			}
			else {
				x = x0 + 1 - n % 3 - n / 3;
				y = y0 - 1 - n / 3;
				if (y == 0){
					bitset<2> bs2{0};
					ifs >> bs2;
					promo = (promo_ty)bs2.to_ulong();
				}
			}
		break;
		case knight:
			group = n >> 1;
			x = x0 - 2 + (group >> 1) * 3 + n & 1;
			y = y0 - 2 + (group & 1) * 3 + ((n & 1) ^ !(group % 3));
		break;
		case king:
			//if (n > 7 && white_turn){  // then end type
			//	extra_type = (extra_ty) (n - 8);				
			//}
			x = x0 - 1 + (n + (n > 3)) % 3;
			y = y0 - 1 + (n + (n > 3)) / 3;
		break;
		case bishop:
			if (n > 7) {
				x = 15 - n;  // 31 - n
				y = y0 + x0 - x;
			}
			else {
				x = n;  // n - 16
				y = y0 - x0 + x;
			}
		break;
		case castle:
			if (n > 7) {
				x = x0;
				y = n - 8;
			}
			else {
				x = n;
				y = y0;
			}
		break;
		case queen:
			switch (n >> 3) {
			case 0:
				x = n;
				y = y0;
			break;
			case 1:
				x = x0;
				y = n - 8;
			break;
			case 2:
				x = n - 16;
				y = y0 - x0 + x;
			break;
			case 3:
				x = 31 - n;
				y = y0 + x0 - x;
			}
		}

		to_coord.first = x;
		to_coord.second = y;

		//white_pieces_by_idx white_pieces_coords  idxs_on_board

		// UPDATE white_pieces_coords[][]
		int to_idx = idxs_on_board[x][y];
		if (to_idx != -1){
			// FIX: need to change idx so that by idx I could understand if it is white
			// or black piece and in the coding just do minus 16.
			// P.S. but at the moment if consider that all input moves
			// are valid, we can just say that to_idx is the idx of the counter
			// color, because we can't eat our friend piece
			bool to_color;
			if (to_idx < 16)
				to_color = true;
			else 
				to_color = false;
			bool enemy_color = !white_turn;
			if (to_color != enemy_color) {
				cerr << "ERROR: in read_move(): you can't eat your own piece";
				return;
			}
			if (to_color) {
				white_pieces_coords[to_idx] = {-1, -1}; 
				// we ate it, it has no coord now
			}
			else {
				black_pieces_coords[to_idx] = {-1, -1};
			}
		}
		if (white_turn) {
			white_pieces_coords[iPiece_idx] = to_coord;
		}
		else {
			black_pieces_coords[iPiece_idx] = to_coord;
		}

		// UPDATE idxs_on_board
		idxs_on_board[x0][y0] = -1;
		idxs_on_board[x][y] = iPiece_idx;

		// UPDATE white_pieces_by_idx if pawn was promoted
		// FIX: later should change array to vector and update it if only 8 4 2 1
		// pieces remain
		if (promo != promo_ty::no_promotion) {
			if (white_turn){
				white_pieces_by_idx[iPiece_idx] = (piece_ty) promo;
			}
			else {
				black_pieces_by_idx[iPiece_idx] = (piece_ty) promo;
			}
		}
		white_turn = !white_turn;

	}
	void write_move(
		pair <int, int> from_coord, 
		pair <int, int> to_coord, 
		promo_ty promo, 
		fsm::ofinestream& ofs) 
	{

		int iPiece_idx = idxs_on_board[from_coord.first][from_coord.second];
		piece_idx = bitset<4> (iPiece_idx);
		ofs << piece_idx;

		piece_ty
			piece_type = white_turn ? white_pieces_by_idx[iPiece_idx] : black_pieces_by_idx[iPiece_idx];
		int 
			x1 = to_coord.first, 
			x0 = from_coord.first, 
			y1 = to_coord.second, 
			y0 = from_coord.second,
			X = x1 - x0,
			Y = y1 - y0,
			n;

		switch(int xstrt, ystrt; piece_type){
			case pawn:
				if (white_turn)
					n = Y == 2 ? 3 : X + 1;
				else
					n = Y == -2 ? 3 : 1 - X;
			break;
			case knight:
				xstrt = x0 - 2;
				ystrt = y0 - 2;
				X = x1 - xstrt;
				Y = y1 - ystrt;
				switch(X){
					case 0:
						if (Y == 1)
							n = 0;
						else if (Y == 3)
							n = 2;
					break;
					case 1:
						if (Y == 0)
							n = 1;
						else if (Y == 4)
							n = 3;
					break;
					case 3:
						if (Y == 0)
							n = 4;
						else if (Y == 4)
							n = 6;
					break;
					case 4:
						if (Y == 1)
							n = 5;
						else if (Y == 3)
							n = 7;
				}
			break;
			case king:
				//if (white_turn && extra_type != usual_move){  // then end type
				//	n = 8 + (int) extra_type;
				//}
				//else{
					xstrt = x0 - 1;
					ystrt = y0 - 1;
					X = x1 - xstrt;
					Y = y1 - ystrt;
					n = X * 3 + Y;
					if (n > 4) n--;					
				//}
			break;
			case castle:
				n = Y ? y1 + 8 : x1;
			break;
			case bishop:
				n = X == Y ? x1 : 15 - x1;
			break;
			case queen:
				if (!Y)
					n = x1;
				else if (!X)
					n = y1 + 8;
				else if (X == Y)
					n = x1 + 16;
				else
					n = 31 - x1;
		}
		int 
			size = move_sizes[piece_type];

		//if (white_turn && piece_type == king)
		//	size++;
		move = { n, size, false };
		// add an operator = to bitremedy, so that there would be no need to do checks manually
		// check if assigning of CBYTE is OK here
		// check that the stream doesn't change aligning of bitredmedy
		ofs << move;
		if (piece_type == pawn && (white_turn && y1 == 7 || !white_turn && y1 == 0)) {
			if (promo == no_promotion) {
				cerr << "ERROR: you need to choose a piece to transform pawn to." << endl;
			}
			ofs << bitset<2>(promo);
		}

		white_turn = !white_turn;
	};
};
void print_chess_coords(pair<int, int> coord) {
	cout << char('a' + coord.first) << coord.second + 1 << endl;
}
//int main() {
//	//Move 
//	//	moveobj;
//	//pair<int, int> 
//	//	from_coord,
//	//	to_coord;
//	//transform_ty
//	//	promotion_type;
//	//extra_ty
//	//	additional_info;
//	////bool is_valid{ true };
//	////moveobj.validate_move(is_valid, pawn, { 0, 1 }, { 1, 8 });
//	////cout << is_valid << endl;
//
//	////fsm::ofinestream ofs("output.txt");
//	////for (int i = 0; i < 20; i++) {
//	////	moveobj.write_move(0, { 0, 3 }, no_promotion, usual_move, ofs);
//	////	moveobj.write_move(1, { 0, 5 }, no_promotion, usual_move, ofs);
//	////	moveobj.write_move(8, { 0, 4 }, no_promotion, usual_move, ofs);
//	////	moveobj.write_move(14, { 6, 4 }, no_promotion, usual_move, ofs);
//	////}
//	//	//moveobj.write_move(0, { 0, 3 }, no_promotion, usual_move, ofs);
//	//	//moveobj.write_move(1, { 0, 5 }, no_promotion, usual_move, ofs);
//	//	//moveobj.write_move(8, { 0, 3 }, no_promotion, usual_move, ofs);
//	//	//moveobj.write_move(14, { 6, 4 }, no_promotion, usual_move, ofs);
//
//	////fsm::ifinestream 
//	////	ifs("output.txt");
//	////transform_ty 
//	////	promotion_type;
//	////extra_ty 
//	////	additional_info;
//	////moveobj.read_move(piece_idx, to_coords, promotion_type, additional_info, ifs);
//	////cout << piece_names[pieces_by_idx[piece_idx]] << endl;
//	////print_chess_coords(to_coords);
//	////moveobj.read_move(piece_idx, to_coords, promotion_type, additional_info, ifs);
//	////cout << piece_names[pieces_by_idx[piece_idx]] << endl;
//	////print_chess_coords(to_coords);
//	// // 
//
//	//fsm::ifinestream 
//	//	ifs("output.txt");
//	//while (!ifs.Eof()) {
//	//	moveobj.read_move(from_coord, to_coord, promotion_type, additional_info, ifs);
//	//	cout << piece_names[pieces_by_idx[idxs_on_board[from_coord.first][from_coord.second] ]] << endl;
//	//	print_chess_coords(to_coord);
//	//}
//}
// TODO: write validate_move with a bunch of lambdas inside of it


// union move_ty {
// 	bitset <2> bs2;
// 	bitset <3> bs3;
// 	bitset <4> bs4;
// 	bitset <5> bs5;
// };


	//void validate_move(bool& result, piece_ty piece_type, pair <int, int> from_coord, pair <int, int> to_coord) {
	//	int X = to_coord.first - from_coord.first,
	//		Y = to_coord.second - to_coord.second;
	//	for (auto el : { from_coord.first, from_coord.second, to_coord.first, to_coord.second })
	//		if (el > 7 || el < 0) {
	//			result = false;
	//			return;
	//		}

	//	switch (piece_type) {
	//	case pawn:
	//		
	//		break;
	//	case knight:
	//		break;
	//	case king:
	//		break;
	//	case bishop:
	//		break;
	//	case castle:
	//		break;
	//	case queen:
	//		break;
	//	}
	//}