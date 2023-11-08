#pragma once
#include "tile.h"
#include "validation.h"
#include "board.h"

Tile::Tile(Board* mother_board, Qt::WindowFlags f) :
    m_board(mother_board), QLabel(mother_board->m_background, f)
{};

void Tile::mousePressEvent(QMouseEvent *event)
{
    emit tileClicked(this);
    //validMove(this,++m_valid_n);
}

void Tile::drawPiece(char elem)
{
    this->m_piece_name=elem;

//    if (!this->m_piece){  // if the m_tiles is empty
//        this->clear();
//        return;
//    }

    if (this->m_white_piece){  // if color is white
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
        this->setPixmap(piece.scaled(this->width(), this->height()));
    }
    else{  // if color is black
        QPixmap piece;
        switch (elem)
        {
        case 'P': piece = QPixmap(":/pawn_black");
            break;
        case 'R': piece = QPixmap(":/rook_black");
            break;
        case 'N': piece = QPixmap(":/knight_black");
            break;
        case 'K': piece = QPixmap(":/king_black");
            break;
        case 'Q': piece = QPixmap(":/queen_black");
            break;
        case 'B': piece = QPixmap(":/bishop_black");
            break;
        }
        if (piece.isNull()) {
            qDebug() << "Can't get acces to pieces images. Use pseudonames "
                        "for the files. Add them to .qrc file if you've changed the project's"
                        "structure.";
        }
        this->setPixmap(piece.scaled(this->width(), this->height()));
    }
}

void Tile::dyeNormal()
{

    if(this->m_white_tile)
        this->setStyleSheet("QLabel {background-color: rgb(120, 120, 90);}:hover{background-color: rgb(170,85,127);}");
    // FIX: all rgb colors should be fields of the Board class int the future, 
    // when functionality will be added
    else
        this->setStyleSheet("QLabel {background-color: rgb(211, 211, 158);}:hover{background-color: rgb(170,95,127);}");
}

