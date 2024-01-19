#pragma once
#include "tile.h"
#include "board.h"
#include "qdebug.h"
#include "qimage.h"
#include "qnamespace.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qsize.h"
//#include "qsvgrenderer.h"
#include <map>
#include <qglobal.h>
#include <string>

Tile::Tile(Board* mother_board, scoord tile_coord, bool side) :
	QLabel(mother_board, Qt::WindowFlags()),
	coord(tile_coord),
	tile_color((coord.x + coord.y) % 2),
	black{ 120, 120, 90 },
	white{ 211, 211, 158 },
	selected(Qt::green),
	valid(Qt::yellow),
	hover{ 185, 108, 146 }, // previous: {170, 85, 127}
	piece_name('e'), // 'e' for "empty"
	piece_color(false)
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
	QString piece;
	switch (elem)
	{
	case 'P':
		piece = QString(":images/pawn_" + add);
		break;
	case 'R':
		piece = QString(":images/rook_" + add);
		break;
	case 'N':
		piece = QString(":images/knight_" + add);
		break;
	case 'K':
		piece = QString(":images/king_" + add);
        if (color) ((Board*)parent())->white_king = this;
        else ((Board*)parent())->black_king = this;
		break;
	case 'Q':
		piece = QString(":images/queen_" + add);
		break;
	case 'B':
		piece = QString(":images/bishop_" + add);
	}
	if (piece.isNull()) {
		qDebug() << curTime() << "Can't get acces to pieces images. Use pseudonames "
			"for the files. Add them again to .qrc file if you've changed the project's"
			"structure.";
	}

//	QSvgRenderer svgRenderer(piece);  // FIX: should be done once in the constructor
//	// but at the moment speed is OK
//    QSize imageSize(100, 100);
//	QImage image(imageSize, QImage::Format_ARGB32);
//	image.fill(Qt::transparent);
//	QPainter painter(&image);
//	svgRenderer.render(&painter);
//	setPixmap(QPixmap::fromImage(image).scaled(width(), height()));
    setPixmap(piece);
}

void Tile::dyeNormal()
{
	if (tile_color)
		setStyleSheet(QString("QLabel{background-color: %0;}:hover{background-color: %1;}").
			arg(css_colors["normal"][1]).arg(css_colors["hover"][1]));
	else
		setStyleSheet(QString("QLabel{background-color: %0;}:hover{background-color: %1;}").
			arg(css_colors["normal"][0]).arg(css_colors["hover"][0]));
}

void Tile::dyeSelected()
{
	if (tile_color)
		setStyleSheet(QString("QLabel{background-color: %0;}:hover{background-color: %1;}")
			.arg(css_colors["selected"][1]).arg(css_colors["hover"][1]));
	else
		setStyleSheet(QString("QLabel{background-color: %0;}:hover{background-color: %1;}")
			.arg(css_colors["selected"][0]).arg(css_colors["hover"][0]));
}

void Tile::dyeValid()
{
	if (tile_color)
		setStyleSheet(QString("QLabel{background-color: %0;}:hover{background-color: %1;}")
			.arg(css_colors["valid"][1]).arg(css_colors["hover"][1]));
	else
		setStyleSheet(QString("QLabel{background-color: %0;}:hover{background-color: %1;}")
                      .arg(css_colors["valid"][0]).arg(css_colors["hover"][0]));
}

virtu Tile::toVirtu()
{
    return {this, this->piece_color, this->piece_name};
}
