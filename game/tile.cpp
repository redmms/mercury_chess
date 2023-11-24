#pragma once
#include "tile.h"
#include "board.h"
#include "local_types.h"
#include <iostream>

Tile::Tile(Board* mother_board, scoord tile_coord) :
   QLabel(mother_board, Qt::WindowFlags()),
   coord(tile_coord),
   tile_color((coord.x + coord.y) % 2)
{
    css_colors["normal"][0] = black.name(QColor::HexRgb);
    css_colors["normal"][1] = white.name(QColor::HexRgb);
    css_colors["selected"][0] = mixColors(black, selected, 0.5).name(QColor::HexRgb);
    css_colors["selected"][1] = mixColors(white, selected, 0.5).name(QColor::HexRgb);
    css_colors["valid"][0] = mixColors(black, valid, 0.5).name(QColor::HexRgb);
    css_colors["valid"][1] = mixColors(white, valid, 0.5).name(QColor::HexRgb);
    css_colors["hover"][0] = hover.name(QColor::HexRgb);
    css_colors["hover"][1] = hover.name(QColor::HexRgb);

    dyeNormal();
    auto size = mother_board->tile_size;
    setGeometry(size/2 + coord.x * size, size/2 + (7 - coord.y) * size,
                size, size); // size/2 is the indent from the left upper corner
}

void Tile::mousePressEvent(QMouseEvent *event)
{
    emit tileClicked(this);
}

QColor Tile::mixColors(QColor color_a, QColor color_b, float b_coef)
{
    return QColor(
    color_a.red()* (1-b_coef) + color_b.red()*b_coef,
    color_a.green()* (1-b_coef) + color_b.green()*b_coef,
    color_a.blue()* (1-b_coef) + color_b.blue()*b_coef,
    255);
}

void Tile::setPiece(char elem, bool color)
{
    piece_color = color;
    piece_name = elem;
    if (elem == 'e') {
        clear();
        dyeNormal();
        return;
    }
    QString add = color ? "white" : "black";
    QPixmap piece;
    switch (elem)
    {
    case 'P':
        piece = QPixmap(":images/pawn_" + add);
        break;
    case 'R':
        piece = QPixmap(":images/rook_" + add);
        break;
    case 'N':
        piece = QPixmap(":images/knight_" + add);
        break;
    case 'K':
        piece = QPixmap(":images/king_" + add);
        if (color) ((Board*)parent())->white_king = this;
        else ((Board*)parent())->black_king = this;
        break;
    case 'Q':
        piece = QPixmap(":images/queen_" + add);
        break;
    case 'B':
        piece = QPixmap(":images/bishop_" + add);
    }
    if (piece.isNull()) {
        qDebug() << "Can't get acces to pieces images. Use pseudonames "
            "for the files. Add them again to .qrc file if you've changed the project's"
            "structure.";
    }
    setPixmap(piece.scaled(width(), height()));
}

void Tile::dyeNormal()
{
    if(tile_color)
        setStyleSheet(QString("QLabel{background: %0;}:hover{background: %1;}").
                      arg(css_colors["normal"][1]).arg(css_colors["hover"][1]));
    else
        setStyleSheet(QString("QLabel{background: %0;}:hover{background: %1;}").
                      arg(css_colors["normal"][0]).arg(css_colors["hover"][0]));
}

void Tile::dyeSelected()
{
    if (tile_color)
        setStyleSheet(QString("QLabel{background: %0;}:hover{background: %1;}")
            .arg(css_colors["selected"][1]).arg(css_colors["hover"][1]));
    else
        setStyleSheet(QString("QLabel{background: %0;}:hover{background: %1;}")
                        .arg(css_colors["selected"][0]).arg(css_colors["hover"][0]));
}

void Tile::dyeValid()
{
    if(tile_color)
        setStyleSheet(QString("QLabel{background: %0;}:hover{background: %1;}")
                            .arg(css_colors["valid"][1]).arg(css_colors["hover"][1]));
    else
        setStyleSheet(QString("QLabel{background: %0;}:hover{background: %1;}")
                            .arg(css_colors["valid"][0]).arg(css_colors["hover"][0]));
}
