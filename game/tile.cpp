#pragma once
#include "tile.h"
#include "board.h"
#include <QDebug>
using namespace std;

Tile::Tile(Board* parent_, scoord tile_coord, bool side) :
	QLabel(parent_),
	VirtualTile(tile_coord, 'e', false, parent_),
	black{ 120, 120, 90 },
	white{ 211, 211, 158 },
	selected(Qt::green),
	valid(Qt::yellow),
	hover{ 185, 108, 146 }, // previous: {170, 85, 127}
	css("QLabel{background-color: %0;}:hover{background-color: %1;}")
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
	auto size = parent_->tile_size;
	if (side)
		setGeometry(size / 2 + coord.x * size, size / 2 + (7 - coord.y) * size,
			size, size); // size/2 is the indent from the left upper corner
	else
		setGeometry(size / 2 + (7 - coord.x) * size, size / 2 + coord.y * size,
			size, size);
}

void Tile::mousePressEvent(QMouseEvent* event)
{
	emit tileClicked(this);
}

QColor Tile::mixColors(QColor color_a, QColor color_b, float b_coef)
{
	return QColor(
		color_a.red() * (1 - b_coef) + color_b.red() * b_coef,
		color_a.green() * (1 - b_coef) + color_b.green() * b_coef,
		color_a.blue() * (1 - b_coef) + color_b.blue() * b_coef,
		255);
}

void Tile::setPiece(char name, bool color)
{
	piece_name = name;
	piece_color = color;
	if (name == 'e') {
		clear();
		dyeNormal();
		return;
	}
	QString add = color ? "white" : "black";
	QString piece;
	switch (name)
	{
	case 'P':
		piece = ":images/pawn_" + add;
		break;
	case 'R':
		piece = ":images/rook_" + add;
		break;
	case 'N':
		piece = ":images/knight_" + add;
		break;
	case 'K':
		piece = ":images/king_" + add;
        if (color) ((Board*)parent())->white_king = coord;
        else ((Board*)parent())->black_king = coord;
		break;
	case 'Q':
		piece = ":images/queen_" + add;
		break;
	case 'B':
		piece = ":images/bishop_" + add;
	}
	if (piece.isNull()) {
		qDebug() << "Can't get acces to pieces images. Use pseudonames "
			"for the files. Add them again to .qrc file if you've changed the project's"
			"structure.";
	}
    setPixmap(piece);
}

void Tile::dyeNormal()
{
	setStyleSheet(css.arg(css_colors["normal"][tile_color]).arg(css_colors["hover"][tile_color]));
}

void Tile::dyeSelected()
{
	setStyleSheet(css.arg(css_colors["selected"][tile_color]).arg(css_colors["hover"][tile_color]));
}

void Tile::dyeValid()
{
	setStyleSheet(css.arg(css_colors["valid"][tile_color]).arg(css_colors["hover"][tile_color]));
}

VirtualTile Tile::toVirtu()
{
    return {coord, piece_name, piece_color};
}
