#pragma once
#include "../app/local_types.h"

class Board;
class VirtualTile : public QObject {
	Q_OBJECT

public:
	VirtualTile(VirtualBoard* mother_board = 0, scoord coord = scoord());

	scoord coord;
	bool tile_color;
	char piece_name;
	bool piece_color;

	void setPiece(char elem, bool color);
};

struct VirTileCmp
{
	bool operator()(const VirtualTile* l, const VirtualTile* r) const
	{
		return l->coord < r->coord;
	}
};