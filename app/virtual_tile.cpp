#pragma once
#include "virtual_tile.h"
#include "virtual_board.h"
using namespace std;

VirtualTile::VirtualTile(VirtualBoard* mother_board, scoord tile_coord) :
	QObject(mother_board)
	coord(tile_coord),
	tile_color((coord.x + coord.y) % 2),
	piece_name('e'),
	piece_color(false)
{}

void VirtualTile::setPiece(char elem, bool color)
{
	piece_color = color;
	piece_name = elem;
	if (elem == 'K') {
		if (color) ((VirtualBoard*)parent())->white_king = this;
		else ((VirtualBoard*)parent())->black_king = this;
	}
}