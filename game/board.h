#pragma once
#include "local_types.h"
#include <QLabel>
#include <QSettings>
class Tile;
class Validation;
class WebClient;

class Board : public QLabel {
	Q_OBJECT

    const QSettings& settings;

    void drawLetters(bool side);
	void drawNumbers(bool side);
	void drawTiles(bool side);
	void openPromotion(Tile* from);

	friend class Tile;
	friend class WebClient;
protected:
	int tile_size;

	void halfMove(scoord from, scoord to);
	void halfMove(Tile* from, Tile* to);

public:
    Board(QLabel* background, const QSettings& settings);

	Validation* valid;
	Tile* tiles[8][8];
	bool turn;
	bool side;
	Tile* from_tile;
	Tile* white_king;
	Tile* black_king;
	pove last_move;
	pove virtual_move;
	Tile* menu[4];
	QString board_css;
	QString promo_css;
    char last_promotion;

	void saveMove(Tile* from, Tile* to, pove& move);
	void revertMove(pove& move);
	void moveVirtually(Tile* from, Tile* to);
	void moveNormally(Tile* from, Tile* to);
	void castleKing(Tile* king, Tile* destination, Tile* rook);
	void passPawn(Tile* from, Tile* to);

	auto operator [](int i) {
		return tiles[i];
	}

signals:
	void newStatus(tatus status);
	void theEnd(endnum end_type);
	void promotionEnd();
    void moveMade(scoord from, scoord to, char promotion_type);
	//void moveMade(Tile* from, Tile* to);

private slots:
	void reactOnClick(Tile* tile);
	void promotePawn(Tile* tile);
};
