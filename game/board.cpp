#pragma once
#include "board.h"
#include "validation.h"

Board::Board(QLabel* background = 0) :
    QLabel((QWidget*) background->parent()),
    m_tile_size(background->width() / 9)
{
    setGeometry(background->geometry());
    setStyleSheet("background: rgb(170, 170, 125)");
    background->~QLabel();
    drawLetters();
    drawNumbers();
    drawTiles();
};

void Board::reactOnClick(Tile* tile) {
    if (m_from_tile == nullptr) { // then show valid moves
        m_from_tile = tile;
        m_valid.showPossible(tile);
    }
    else if (m_valid.m_valid_moves.contains(tile)) { // then move pieces
        m_valid.hidePossible();
        tile->setPiece(m_from_tile->m_piece_name, m_from_tile->m_white_piece);       
        m_from_tile->setPiece('e', 0);
        m_from_tile = nullptr;
    }
    else if (tile->m_white_piece == m_white_turn) { // then pick another piece
        m_valid.hidePossible();
        m_from_tile = tile;
        m_valid.showPossible(tile);
    }

}

void Board::drawLetters() {
    int width = m_tile_size, height = m_tile_size/2,
        x = m_tile_size / 2, y = this->height() - height;
    for (char ch : "abcdefgh") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(m_tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        x += width;
    }
    x = width / 2, y = 0;
    for (char ch : "abcdefgh") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(m_tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        x += width;
    }
}

void Board::drawNumbers() {
    int width = m_tile_size / 2, height = m_tile_size,
        x = 0, y = m_tile_size / 2;
    for (char ch : "87654321") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(m_tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        y += height;
    }
    x = this->width() - width, y = height / 2;
    for (char ch : "87654321") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(m_tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        y += height;
    }
}

void Board::drawTiles(/*Tile* m_tiles[8][8]*/)
{
    int hor = m_tile_size / 2;
    int ver = m_tile_size / 2;
    int k = 0;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            m_tiles[i][j] = new Tile(this);
            m_tiles[i][j]->m_white_tile = k % 2;
            m_tiles[i][j]->m_tile_num = k++;
            m_tiles[i][j]->m_coord = std::pair(j, i); 
            // is this field really needed?
            // Yes, it is otherwise you can't connect the tile pointer to it's 
            // index, when tileClicked signal is emitted
            // and also we can't use gridlayout because its index goes
            // upside down
            m_tiles[i][j]->setGeometry(hor, ver, m_tile_size, m_tile_size);
            m_tiles[i][j]->dyeNormal();
            hor += m_tile_size;
        }
        ver += m_tile_size;
    }

    //black pawns
    for (int j = 0; j < 8; j++)
        m_tiles[1][j]->setPiece('P', 0);

    //white pawns
    for (int j = 0; j < 8; j++)
        m_tiles[6][j]->setPiece('P', 1);

    m_tiles[0][0]->setPiece('R', 0);
    m_tiles[0][1]->setPiece('N', 0);
    m_tiles[0][2]->setPiece('B', 0);
    m_tiles[0][3]->setPiece('Q', 0);
    m_tiles[0][4]->setPiece('K', 0);
    m_tiles[0][5]->setPiece('B', 0);
    m_tiles[0][6]->setPiece('N', 0);
    m_tiles[0][7]->setPiece('R', 0);

    m_tiles[7][0]->setPiece('R', 1);
    m_tiles[7][1]->setPiece('N', 1);
    m_tiles[7][2]->setPiece('B', 1);
    m_tiles[7][3]->setPiece('Q', 1);
    m_tiles[7][4]->setPiece('K', 1);
    m_tiles[7][5]->setPiece('B', 1);
    m_tiles[7][6]->setPiece('N', 1);
    m_tiles[7][7]->setPiece('R', 1);
}