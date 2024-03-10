#pragma once
#include "virtual_board.h"
#include "validator.h"
#include <QLabel>

class VirtualTile;
class Tile;
class WebClient;
class MainWindow;
class Board : public QLabel, public VirtualBoard {
	Q_OBJECT

public:
	MainWindow* mainwindow;
    Validator valid;
    Tile* tiles[8][8];
    Tile* menu[4];
	QString board_css;
	QString promo_css;
	int tile_size;
	std::vector<bitmove> bistory;

	Board(MainWindow* parent_, QLabel* background_);
	~Board();

	void initLetter(int x, int y, int width, int height, QString ch);
	void drawLetters();
	void drawNumbers();
	char openPromotion(scoord from);
	void halfMove(scoord from, scoord to, char promo);
	void halfMove(scoord from, scoord to, char promo, halfmove& saved_hmove); 
	// one of these 3 functions will be called in reactOnClick depending on settings
	void halfMove(scoord from, scoord to, char promo, bitmove& saved_bmove, halfmove& saved_hmove);

	// virtual methods
	Tile* theTile(scoord coord) override;
	void initTiles() override;

signals:
	void moveMade(scoord from, scoord to, char promotion_type);
	void theEnd(endnum end_type);
	void newStatus(tatus status);
	void promotionEnd();

public slots:
	void reactOnClick(Tile* tile);
	void promotePawn(scoord from, char into);
};
