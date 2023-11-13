#pragma once
#include "tile.h"
#include "validation.h"
enum endnum : int;

class Board : public QLabel {
	Q_OBJECT

    Validation m_valid = Validation(this);
    int m_tile_size;

    void drawLetters();
    void drawNumbers();
    void drawTiles();
    //void drawPromotion();

public:
    Board(QLabel* background);
    
    Tile* m_tiles[8][8] = { { NULL } };
    bool m_white_turn = true;
    Tile* m_from_tile = nullptr;
    Tile* m_white_king = nullptr;
    Tile* m_black_king = nullptr;

    auto operator [](int i) {
        return m_tiles[i];
    }
    
signals:
    void newStatus(QString status);
    void theEnd(endnum end_type);

private slots:
    void reactOnClick(Tile* tile);

};
