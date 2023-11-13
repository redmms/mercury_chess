#pragma once
#include "board.h"
#include <QGridLayout>
#include "enums.h"



Board::Board(QLabel* background = 0) :
    m_tile_size(background->width() / 9)
{
    setGeometry(background->geometry());   // replace ui board, by this class
    setStyleSheet("background: rgb(170, 170, 120);");
    background->parentWidget()->layout()->replaceWidget(background, this);
    background->~QLabel();

    drawLetters();
    drawNumbers();
    drawTiles();
};

void Board::reactOnClick(Tile* tile) {
    if (m_from_tile == nullptr) { // if it's first click then pick the piece and 
    // show valid moves
        if (m_white_turn != tile->m_white_piece || tile->m_piece_name == 'e')
            return;
        // turn for black, but white try to move, or vice versa, or tile is empty
        else {
            m_from_tile = tile;
            m_valid.showValid(tile);
        }
    }
    else if (tile != m_from_tile && tile->m_white_piece == m_white_turn && tile->m_piece_name != 'e') {
    // if the second click
    // is on the piece of same color then pick it instead
        m_valid.hideValid();
        m_from_tile = tile;
        m_valid.showValid(tile);
    }
    else if (m_valid.isValid(tile)) { // if it's the second click and move is valid
    //then move pieces
        m_valid.hideValid();
        tile->setPiece(m_from_tile->m_piece_name, m_from_tile->m_white_piece);       
        m_from_tile->setPiece('e', 0);
        m_from_tile = nullptr;
        m_white_turn = !m_white_turn;
        Tile* king = m_white_turn ? m_white_king : m_black_king;
        if (m_valid.inCheck(king)){
            if (m_valid.inCheckmate(king)){
                if (m_white_turn){
                    emit newStatus("Black win");
                    emit theEnd(endnum::black_wins);
                    return;
                }
                else{
                    emit newStatus("White win");
                    emit theEnd(endnum::white_wins);
                    return;
                }
            }
            else{
                emit newStatus("Check! Save your king");
                return;
                }
        }
        else if(m_valid.inStalemate(m_white_turn)){
            emit newStatus("Draw");
            emit theEnd(endnum::draw);
            return;
        }
        // FIX: here should be checks of stalemate, check, and checkmate
        emit newStatus(m_white_turn ? "White turn" : "Black turn");
    }
    else{
        emit newStatus("Invalid move");
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

void Board::drawTiles()
{
    int indent = m_tile_size / 2;
    int hor = indent;
    int ver = indent;
    for (int y = 7; y >= 0; y--){
        for (int x = 0; x < 8; x++){
            m_tiles[x][y] = new Tile(this);
            m_tiles[x][y]->m_white_tile = !((x + y) % 2);
            m_tiles[x][y]->m_tile_num = y * 8 + x;
            // is this field really needed?
// Yes, it is otherwise you can't connect the tile pointer to it's 
// index, when tileClicked signal is emitted
// and also we can't use gridlayout because its index goes
// upside down
            m_tiles[x][y]->m_coord = {x, y}; 
            m_tiles[x][y]->setGeometry(hor, ver, m_tile_size, m_tile_size);
            m_tiles[x][y]->dyeNormal();
            QObject::connect(m_tiles[x][y], &Tile::tileClicked, this, &Board::reactOnClick);
            hor += m_tile_size;
        }
        hor = indent;
        ver += m_tile_size;
    }

    //black pawns
    for (int x = 0; x < 8; x++)
        m_tiles[x][6]->setPiece('P', 0);

    //white pawns
    for (int x = 0; x < 8; x++)
        m_tiles[x][1]->setPiece('P', 1);

    m_tiles[0][7]->setPiece('R', 0);
    m_tiles[1][7]->setPiece('N', 0);
    m_tiles[2][7]->setPiece('B', 0);
    m_tiles[3][7]->setPiece('Q', 0);
    m_tiles[4][7]->setPiece('K', 0);
    m_tiles[5][7]->setPiece('B', 0);
    m_tiles[6][7]->setPiece('N', 0);
    m_tiles[7][7]->setPiece('R', 0);

    m_tiles[0][0]->setPiece('R', 1);
    m_tiles[1][0]->setPiece('N', 1);
    m_tiles[2][0]->setPiece('B', 1);
    m_tiles[3][0]->setPiece('Q', 1);
    m_tiles[4][0]->setPiece('K', 1);
    m_tiles[5][0]->setPiece('B', 1);
    m_tiles[6][0]->setPiece('N', 1);
    m_tiles[7][0]->setPiece('R', 1);
}