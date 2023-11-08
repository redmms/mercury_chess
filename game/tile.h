#ifndef TILE_H
#define TILE_H
#include <QLabel>
#include <QDebug>

class Board;
class Tile: public QLabel
{
    Q_OBJECT

    void mousePressEvent(QMouseEvent* event); // FIX: doesn't work

public:

    Board* m_board = nullptr;
    int
        m_col = 0,
        m_row = 0,
        m_tile_num = 0;
    bool 
//m_white_turn = true, // should be in the board class, and the tile just 
// emits the signal that it is pressed
        m_white_tile = 0,
        m_has_piece = 0, // FIX: can be omitted, use m_piece_name == 0 instead
        m_white_piece;
//Tile* m_from_tile = nullptr; // FIX: should be passed as a parameter
// // Validation* m_validity = nullptr; // FIX: should be in the board class
    char m_piece_name = 0;

    Tile(Board* board_background = 0, Qt::WindowFlags f = Qt::WindowFlags());

    void drawPiece(char elem);
    void dyeNormal();
// void validMove(Tile* temp, int c); // FIX: doesn't work

signals:

    void tileClicked(Tile* tile); // Объявление сигнала

};

#endif // TILE_H
