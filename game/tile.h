#ifndef TILE_H
#define TILE_H
#include <QLabel>
#include <QDebug>
#include "local_types.h"
#include <QMap>
#include <QColor>
#include <map>
#include <string>
class Board;

class Tile: public QLabel
{
    Q_OBJECT

    QColor black{120, 120, 90};
    QColor white{211, 211, 158};
    QColor selected = Qt::green;
    QColor valid = Qt::yellow;
    QColor hover{185, 108, 146};    //{170, 85, 127};
    std::map<std::string, std::map<int, QString>> css_colors;

    void mousePressEvent(QMouseEvent* event); 
    QColor mixColors(QColor color_a, QColor color_b, float b_coef);

public:
    Tile(Board* mother_board = 0, scoord coord = {0, 0});

    scoord coord;
    bool tile_color = false;
    char piece_name = 'e'; // "e" for "empty"
    bool piece_color = false;

    void setPiece(char elem, bool color);
    void dyeNormal();
    void dyeSelected();
    void dyeValid();

signals:
    void tileClicked(Tile* tile);
};

#endif // TILE_H
