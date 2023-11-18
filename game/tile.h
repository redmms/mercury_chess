#ifndef TILE_H
#define TILE_H
#include <QLabel>
#include <QDebug>
#include "local_types.h"

class Board;
//struct scoord;

class Tile: public QLabel
{
    Q_OBJECT
   
    void mousePressEvent(QMouseEvent* event); 

public:
    Tile(Board* mother_board);

    scoord coord;
    bool tile_color = false;
    char piece_name = 'e'; // "e" for "empty"
    bool piece_color = false;

    void setPiece(char elem, bool color);
    void dyeNormal();

signals:
    void tileClicked(Tile* tile);
};

#endif // TILE_H
