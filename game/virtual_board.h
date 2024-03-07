#pragma once
#include "../app/local_types.h"
#include "virtual_validator.h"
#include "virtual_tile.h"

class VirtualBoard : public QObject {
	Q_OBJECT

public:
	VirtualValidator valid;
	VirtualTile tiles[8][8];
	VirtualTile* from_tile;
	VirtualTile* white_king;
	VirtualTile* black_king;
	bool turn;
	bool side;
	int current_move;
	endnum end_type;
	std::vector<halfmove> history;

	VirtualBoard(QObject* parent_);

	// move forward
	void saveMoveNormally(scoord from, scoord to, vove& move);
	void moveNormally(scoord from, scoord to);
	void castleKing(scoord king, scoord destination, scoord rook);
	void passPawn(scoord from, scoord to);
	void halfMove(scoord from, scoord to, char promo = 'e', bool save_story = true);
	void doCurrentMove();

	// move backward (undo)
	void restoreTile(VirtualTile saved);
	void revertMoveNormally(vove move);
	void revertCastling(vove move);
	void revertPass(vove move);
	void revertPromotion(vove move);
	void revertHalfmove(halfmove move);
	void revertCurrentMove();

	// virtual methods
	virtual VirtualTile* theTile(scoord coord);
	virtual VirtualTile* theTile(VirtualTile tile);
	virtual void setTiles();
	virtual void promotePawn(scoord from, char into);

	auto operator [](int i) {
		return tiles[i];
	}

signals:
	void moveMade(scoord from, scoord to, char promotion_type);
	void theEnd(endnum end_type);
};