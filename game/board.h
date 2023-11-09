#pragma once
#include "tile.h"
class Validation;

class Board : public QLabel {

    Validation m_valid = Validation (this);
    int m_tile_size;

    void drawLetters();
    void drawNumbers();
    void drawTiles();

public:
    Board(QLabel* background);

    Tile* m_tiles[8][8] = { { NULL } };
    bool m_white_turn = true;
    Tile* m_from_tile = nullptr;

    auto operator [](int i) {
        return m_tiles[i];
    }

private slots:
    void reactOnClick(Tile* tile);

};
