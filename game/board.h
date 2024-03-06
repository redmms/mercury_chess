#pragma once
#include "../app/local_types.h"
#include "validator.h"
#include "virtual_tile.h"
#include <QLabel>

class WebClient;
class MainWindow;
class Tile;
class Board : public QLabel {
	Q_OBJECT
public:

    void drawLetters(bool side);
	void drawNumbers(bool side);
	void drawTiles(bool side);
	void openPromotion(Tile* from);

    Board(QLabel* background, MainWindow* mainwindow_);
	~Board();

	QPointer<MainWindow> mainwindow;
    std::vector<halfmove> history;
	std::vector<bitmove> bistory;
    Validator valid;
    Tile* tiles[8][8];
	bool turn;
	bool side;
    Tile* from_tile;
    Tile* white_king;
    Tile* black_king;
	vove virtual_move;
    Tile* menu[4];
	QString board_css;
	QString promo_css;
    char last_promotion;
    VirtualTile last_virtually_passed;
	endnum end_type;
	int tile_size;

	void halfMove(scoord from, scoord to);
	void halfMove(Tile* from, Tile* to);
	void saveMoveNormally(Tile* from, Tile* to, vove& move);
	void moveNormally(Tile* from, Tile* to);
	void castleKing(Tile* king, Tile* destination, Tile* rook);
	void passPawn(Tile* from, Tile* to);
    void restoreTile(VirtualTile saved);
	void promotePawn(Tile* from, char into);
	void promotePawn(scoord from, char into);
	void moveVirtually(Tile* from, Tile* to, vove& move);
	void revertVirtualMove(vove& move);
	bitmove toBitmove(halfmove hmove);
	bitremedy toPieceIdx(Tile* from);
	bitremedy toMoveIdx(Tile* to);
	Tile* theTile(scoord);
	Tile* theTile(VirtualTile);
    //int idx(scoord coord);

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
    void promotePawn(Tile* from, Tile* into);
};
