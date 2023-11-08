#pragma once
#include "board.h"
#include "validation.h"

Board::Board(QLabel* board_ptr = 0) :
    m_background(board_ptr),
    m_tile_size(board_ptr->width() / 9)
{
    drawLetters();
    drawNumbers();
    drawTiles();
};

void Board::reactOnClick(Tile* tile) {
    if (m_from_tile == nullptr) { // then show valid moves
        m_from_tile = tile;
        m_valid = new Validation(this);
        m_valid->findValid(tile);
        m_valid->dyeOrange();
        tile->setStyleSheet("background: green;");
        // FIX: should be Validation::showPossible()
    }
    else if (m_valid->validMove(m_from_tile, tile)) { // then move pieces
        m_from_tile->m_has_piece = false;
        m_from_tile->dyeNormal();
        // FIX: should be part of setPiece
        // for the case of piece_name == 'E' //for empty
        m_from_tile = nullptr;
        tile->m_has_piece = true;
        tile->m_white_piece = m_from_tile->m_white_piece;
        tile->m_piece_name = m_from_tile->m_piece_name;
        tile->drawPiece(m_from_tile->m_piece_name); 
        // FIX: instead of drawPiece
        // should be setPiece();
        // which will include all this:
        //        tile->m_has_piece = true;
        //tile->m_white_piece = m_from_tile->m_white_piece;
        //tile->m_piece_name = m_from_tile->m_piece_name;
        //tile->drawPiece(m_from_tile->m_piece_name);
        m_valid->disOrange();
        m_valid->m_valid_n = 0;
        m_valid->m_valid_moves = { {nullptr} };
        // FIX: should be 
        // Validation::disValidate()
        
    }
    else if (tile->m_white_piece == m_white_turn) { // then change the current
        // m_from_tile and show new valid moves

    }

}

void Board::drawLetters() {
    int width = m_tile_size, height = m_tile_size/2,
        x = m_tile_size / 2, y = m_background->height() - height;
    for (char ch : "abcdefgh") {
        QLabel* letter = new QLabel(m_background);
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
        QLabel* letter = new QLabel(m_background);
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
        QLabel* letter = new QLabel(m_background);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(m_tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        y += height;
    }
    x = m_background->width() - width, y = height / 2;
    for (char ch : "87654321") {
        QLabel* letter = new QLabel(m_background);
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
    //Create 64 m_tiles (allocating memories to the objects of Tile class)
    int ver = m_tile_size/2;
    int k = 0;
    for (int i = 0; i < 8; i++)
    {
        int hor = m_tile_size/2;
        for (int j = 0; j < 8; j++)
        {
            m_tiles[i][j] = new Tile(this);
            m_tiles[i][j]->m_white_tile = (i + j) % 2;
            m_tiles[i][j]->m_has_piece = 0;
            m_tiles[i][j]->m_row = i; // is this field really needed?
            // Yes, it is otherwise you can't connect the tile pointer to it's 
            // index, when tileClicked signal is emitted
            // and also we can't use gridlayout because its index goes
            // upside down
            m_tiles[i][j]->m_col = j;
            m_tiles[i][j]->m_tile_num = k++;
            m_tiles[i][j]->setGeometry(hor, ver, m_tile_size, m_tile_size);
            m_tiles[i][j]->dyeNormal();
            hor += m_tile_size;
        }
        ver += m_tile_size;
    }

    //white pawns
    // FIX: m_piece_color == 0 - is for white pieces
    // piece == 1 - means there's some piece on the tile

    for (int j = 0; j < 8; j++)
    {
        m_tiles[1][j]->m_has_piece = 1;
        m_tiles[1][j]->m_white_piece = 0;
        m_tiles[1][j]->drawPiece('P');
    }

    //black pawns
    for (int j = 0; j < 8; j++)
    {
        m_tiles[6][j]->m_has_piece = 1;
        m_tiles[6][j]->m_white_piece = 1;
        m_tiles[6][j]->drawPiece('P');
    }

    //white and black remaining elements
    for (int j = 0; j < 8; j++)
    {
        m_tiles[0][j]->m_has_piece = 1;
        m_tiles[0][j]->m_white_piece = 0;
        m_tiles[7][j]->m_has_piece = 1;
        m_tiles[7][j]->m_white_piece = 1;
    }


        m_tiles[0][0]->drawPiece('R');
        m_tiles[0][1]->drawPiece('N');
        m_tiles[0][2]->drawPiece('B');
        m_tiles[0][3]->drawPiece('Q');
        m_tiles[0][4]->drawPiece('K');
        m_tiles[0][5]->drawPiece('B');
        m_tiles[0][6]->drawPiece('N');
        m_tiles[0][7]->drawPiece('R');



        m_tiles[7][0]->drawPiece('R');
        m_tiles[7][1]->drawPiece('N');
        m_tiles[7][2]->drawPiece('B');
        m_tiles[7][3]->drawPiece('Q');  // FIX: how this method distinguish if it a black piece or white?
        m_tiles[7][4]->drawPiece('K');
        m_tiles[7][5]->drawPiece('B');
        m_tiles[7][6]->drawPiece('N');
        m_tiles[7][7]->drawPiece('R');

}
