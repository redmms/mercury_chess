#pragma once
#include "tile.h"
#include "validation.h"
#include "local_types.h"
#include <QEventLoop>
enum endnum : int;

class Board : public QLabel {
	Q_OBJECT

    Validation valid = Validation(this);
    int tile_size;

    void drawLetters();
    void drawNumbers();
    void drawTiles();
    void openPromotion(Tile* from);

public:
    Board(QLabel* background);
    
    Tile* tiles[8][8] = { { NULL } };
    bool turn = true;  // true for white turn;
    Tile* from_tile = nullptr;  // always actualized in Tile::setPiece()
    Tile* white_king = nullptr;  // dito
    Tile* black_king = nullptr;
    pove last_move;
    pove virtual_move;
    Tile* menu[4];

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
    void newStatus(setatus status);
    void theEnd(endnum end_type);
    void promotionEnd();
    void hasMoved(Tile* from, Tile* to);

private slots:
    void reactOnClick(Tile* tile);
    void promotePawn(Tile* tile);
};
