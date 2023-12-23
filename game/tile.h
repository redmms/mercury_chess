#ifndef TILE_H
#define TILE_H
#include "local_types.h"
#include "qcolor.h"
#include "qlabel.h"
#include "qstring.h"
#include <map>
#include <string>
class Board;
class QMouseEvent;

class Tile : public QLabel
{
	Q_OBJECT

	QColor black;
	QColor white;
	QColor selected;
	QColor valid;
	QColor hover;
	std::map<std::string, std::map<int, QString>> css_colors;

	void mousePressEvent(QMouseEvent* event);
	QColor mixColors(QColor color_a, QColor color_b, float b_coef);

public:
	Tile(Board* mother_board = 0, scoord coord = { 0, 0 }, bool side = true);

	scoord coord;
	bool tile_color;
	char piece_name;
	bool piece_color;

	void setPiece(char elem, bool color);
	void dyeNormal();
	void dyeSelected();
	void dyeValid();

signals:
	void tileClicked(Tile* tile);
};

#endif // TILE_H
