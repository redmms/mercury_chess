#pragma once
#include "virtual_tile.h"

struct VirtualTileContainer {
	VirtualTile* tiles[8][8];
	operator [](int i) {
		return tiles[i];
	}
};