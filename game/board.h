#pragma once
#include "validator.h"
#include "tile.h"
#include "../app/local_types.hpp"
#include <QLabel>
#include <QSettings>
#include <QPointer>
#include <QDebug>
#include <vector>
import bitremedy;
import finestream;

class WebClient;
class MainWindow;
class Board : public QLabel {
	Q_OBJECT
public:

    void drawLetters(bool side);
	void drawNumbers(bool side);
	void drawTiles(bool side);
	void openPromotion(Tile* from);

    Board(QLabel* background, QSettings& settings_, MainWindow* mainwindow_);

	QPointer<MainWindow> mainwindow;
    std::vector<halfmove> history;
	std::vector<bitmove> bistory;
    Validator* valid;
    QPointer<Tile> tiles[8][8];
	bool turn;
	bool side;
    QPointer<Tile> from_tile;
    QPointer<Tile> white_king;
    QPointer<Tile> black_king;
	pove virtual_move;
    QPointer<Tile> menu[4];
	QString board_css;
	QString promo_css;
    char last_promotion;
    virtu last_virtually_passed;
	endnum end_type;
	int tile_size;
	QPointer<QSettings> settings;

	void halfMove(scoord from, scoord to);
	void halfMove(Tile* from, Tile* to);
	void saveMove(Tile* from, Tile* to, pove& move);
    void revertVirtualMove(pove& move);
    void moveVirtually(Tile* from, Tile* to, pove& move);
	void moveNormally(Tile* from, Tile* to);
	void castleKing(Tile* king, Tile* destination, Tile* rook);
	void passPawn(Tile* from, Tile* to);
    void restoreTile(virtu saved);
	void promotePawn(Tile* from, char into);
	bitmove toBitmove(halfmove hmove);
	bitremedy toPieceIdx(Tile* from);
	bitremedy toMoveIdx(Tile* to);
    //int idx(scoord coord);

	auto operator [](int i) {
        return tiles[i];
    }

signals:
	void newStatus(tatus status);
	void theEnd(endnum end_type);
	void promotionEnd();
    void moveMade(scoord from, scoord to, char promotion_type);
	void needBoardUpdate();
	//void moveMade(Tile* from, Tile* to);

private slots:
	void reactOnClick(Tile* tile);
    void promotePawn(Tile* from, Tile* into);
};
