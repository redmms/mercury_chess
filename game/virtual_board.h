#pragma once
#include "../app/local_types.h"
#include "virtual_validator.h"
#include "virtual_tile.h"
#include <string>

class VirtualBoard {
public:
	VirtualValidator vvalid;
	VirtualTile* tiles[8][8];
	scoord from_coord;
	scoord white_king;
	scoord black_king;
	bool turn = true;
	bool side;
	int current_move;
	endnum end_type;
	std::vector<halfmove> history;

	VirtualBoard();

	// move forward
	void saveMoveNormally(scoord from, scoord to, vove& move);
	void moveNormally(scoord from, scoord to, bool virtually = false);
	void castleKing(scoord king, scoord destination, scoord rook, bool virtually = false);
	void passPawn(scoord from, scoord to, bool virtually = false);
	virtual void promotePawn(scoord from, char into, bool virtually = true);
	void halfMove(scoord from, scoord to, char promo, endnum& end_type);
	void halfMove(scoord from, scoord to, char promo, endnum& end_type, halfmove& saved, bool virtually = false);

	// move backward (undo)
	void restoreTile(const VirtualTile& saved, bool virtually = false);
	void revertMoveNormally(vove move, bool virtually = false);
	void revertCastling(vove move, bool virtually = false);
	void revertPass(vove move, bool virtually = false);
	void revertPromotion(vove move, bool virtually = false);
	void revertHalfmove(halfmove saved, bool virtually = false);

	// methods for watching history
	void doCurrentMove();
	void revertCurrentMove();

	// tile initialization
	virtual void initTiles();
	void setTiles();

	// tile access
	 virtual VirtualTile* theTile(scoord coord);
	 //virtual VirtualTile* overTile(scoord coord, bool virtually = false);

	 std::string toStr();
};