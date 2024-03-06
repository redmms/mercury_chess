#pragma once
#include "../app/local_types.h"
#include "virtual_validator.h"
#include "virtual_tile.h"

class VirtualBoard : public QObject {
	Q_OBJECT

public:
	void setTiles();

	VirtualBoard(QObject* parent/*, bool save_story*/);

	std::vector<halfmove> history;
	VirtualValidator valid;
	VirtualTile tiles[8][8];
	bool turn;
	bool side;
	VirtualTile* from_tile;
	VirtualTile* white_king;
	VirtualTile* black_king;
	char last_promotion;
	endnum end_type;
	int current_move;


	// move forward
	void moveNormally(VirtualTile& from, VirtualTile& to);
	void castleKing(VirtualTile& king, VirtualTile& destination, VirtualTile& rook);
	void passPawn(VirtualTile& from, VirtualTile& to);
	void promotePawn(VirtualTile& from, char into);
	void promotePawn(scoord from, char into);
	void halfMove(scoord from, scoord to, char promo = 'e');
	void halfMove(VirtualTile& from, VirtualTile& to, char promo = 'e');
	void doCurrentMove();

	// move backward (undo)
	void restoreTile(VirtualTile& restored, VirtualTile saved);
	void revertMoveNormally(vove& move);
	void revertCastling(vove move);
	void revertPass(vove move);
	void revertPromotion(vove move);
	void revertHalfmove(halfmove move);
	void revertCurrentMove();
	void revertLastMove();
	VirtualTile& theTile(scoord coord);
	VirtualTile& theTile(VirtualTile tile);

	auto operator [](int i) {
		return tiles[i];
	}

signals:
	//void moveMade(scoord from, scoord to, char promotion_type);
	void theEnd(endnum end_type);
};

//void saveMoveNormally(const VirtualTile& from, const VirtualTile& to, vove& move);
