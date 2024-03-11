#pragma once
#include "virtual_tile.h"
#include <QLabel>

class Board;
class QMouseEvent;
class Tile : public QLabel, public VirtualTile {
	Q_OBJECT

	QColor black;
	QColor white;
	QColor selected;
	QColor valid;
	QColor hover;
	std::map<std::string, std::map<int, QString>> css_colors;
	QString css;

	void mousePressEvent(QMouseEvent* event);
	QColor mixColors(QColor color_a, QColor color_b, float b_coef);

public:
	Tile(Board* mother_board = 0, scoord coord = scoord(), bool side = true);

	void setPiece(char elem, bool color, bool virtually = false) override;
	void dyeNormal();
	void dyeSelected();
	void dyeValid();
    VirtualTile toVirtu();

signals:
	void tileClicked(Tile* tile);
};

//struct TileCmp
//{
//	bool operator()(const Tile* l, const Tile* r) const
//	{
//		return l->coord < r->coord;
//	}
//};