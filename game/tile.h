#ifndef TILE_H
#define TILE_H
#include <QLabel>
#include <QDebug>
#include <utility>
class Board;

class Tile: public QLabel
{
    Q_OBJECT
   
    void mousePressEvent(QMouseEvent* event); 

public:
    Tile(Board* mother_board);

    std::pair<int, int> m_coord = std::pair( 0, 0 );
    int m_tile_num = 0;
    bool m_white_tile = false;
    char m_piece_name = 'e'; // "e" for "empty"
    bool m_white_piece = false;

    void setPiece(char elem, bool color);
    void dyeNormal();

signals:
    void tileClicked(Tile* tile);
};

#endif // TILE_H
