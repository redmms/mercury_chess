#pragma once
#include "tile.h"
#include "validation.h"
#include "local_types.h"
enum endnum : int;
//struct virtu;
//struct scoord;

class Board : public QLabel {
	Q_OBJECT

    Validation valid = Validation(this);
    int tile_size;

    void drawLetters();
    void drawNumbers();
    void drawTiles();
    //void drawPromotion();

public:
    Board(QLabel* background);
    
    Tile* tiles[8][8] = { { NULL } };
    bool turn = true;  // true for white turn;
    Tile* from_tile = nullptr;
    Tile* white_king = nullptr;
    Tile* black_king = nullptr;
    std::pair <virtu, virtu> last_move;
    bool check = false;

    void moveVirtually(scoord from, scoord to);
    void revertLast();

    auto operator [](int i) {
        return tiles[i];
    }
    
signals:
    void newStatus(QString status);
    void theEnd(endnum end_type);

private slots:
    void reactOnClick(Tile* tile);

};
