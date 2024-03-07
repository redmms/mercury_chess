#pragma once
#include "virtual_board.h"
#include "validator.h"

class QLabel;
class VirtualTile;
class Tile;
class WebClient;
class MainWindow;
class Board : public QLabel, public VirtualBoard {
public:

	MainWindow* mainwindow;
    Validator valid;
    Tile* tiles[8][8];
    Tile* from_tile;
    Tile* white_king;
    Tile* black_king;
    Tile* menu[4];
	QString board_css;
	QString promo_css;
	int tile_size;
	std::vector<bitmove> bistory;

	Board(QLabel* background_, MainWindow* parent_);
	~Board();

	void drawLetters(bool side);
	void drawNumbers(bool side);
	void openPromotion(scoord from);
	void moveVirtually(scoord from, scoord to, vove& move);
	void revertVirtualMove(vove move);

	// virtual methods
	Tile* theTile(scoord coord) override;
	Tile* theTile(VirtualTile tile) override;
	void setTiles(bool side) override;

signals:
	void newStatus(tatus status);
	void promotionEnd();

private slots:
	void reactOnClick(scoord tile);
	void promotePawn(scoord from, char into) override;
};
