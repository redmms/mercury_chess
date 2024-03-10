#pragma once
#include "tile.h"

struct TileContainer : {
	Tile* tiles[8][8];
	operator [](int i) {
		return tiles[i];
	}
};