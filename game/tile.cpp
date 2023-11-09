#pragma once
#include "tile.h"
#include "board.h"

Tile::Tile(Board* mother_board = 0, Qt::WindowFlags f = Qt::WindowFlags()) :
    QLabel(mother_board, f)
{};

void Tile::mousePressEvent(QMouseEvent *event)
{
    emit tileClicked(this);
}

void Tile::setPiece(char elem, bool color)
{
    m_white_piece = color;
    m_piece_name = elem;

    if (elem == 'e') {
        clear();
        dyeNormal();
    }
    else if (this->m_white_piece){  // if color is white
        QPixmap piece;
        switch(elem)
        {
            case 'P': piece = QPixmap(":/pawn_white");
                      break;
            case 'R': piece = QPixmap(":/rook_white");
                      break;
            case 'N': piece = QPixmap(":/knight_white");
                      break;
            case 'K': piece = QPixmap(":/king_white");
                      break;
            case 'Q': piece = QPixmap(":/queen_white");
                      break;
            case 'B': piece = QPixmap(":/bishop_white");
                      break;
        }
        setPixmap(piece.scaled(width(), height()));
    }
    else{  // if color is black
        QPixmap piece;
        switch (elem)
        {
        case 'P': 
            piece = QPixmap(":/pawn_black");
        break;
        case 'R': 
            piece = QPixmap(":/rook_black");
        break;
        case 'N': 
            piece = QPixmap(":/knight_black");
        break;
        case 'K': 
            piece = QPixmap(":/king_black");
        break;
        case 'Q': 
            piece = QPixmap(":/queen_black");
        break;
        case 'B': 
            piece = QPixmap(":/bishop_black");
        }
        if (piece.isNull()) {
            qDebug() << "Can't get acces to pieces images. Use pseudonames "
                        "for the files. Add them to .qrc file if you've changed the project's"
                        "structure.";
        }
        setPixmap(piece.scaled(this->width(), this->height()));
    }
}

void Tile::dyeNormal()
{
    if(m_white_tile)
        setStyleSheet("QLabel {background-color: rgb(120, 120, 90);}:hover{background-color: rgb(170,85,127);}");
    // FIX: all rgb colors should be fields of the Board class int the future, 
    // when functionality will be added
    else
        setStyleSheet("QLabel {background-color: rgb(211, 211, 158);}:hover{background-color: rgb(170,95,127);}");
}