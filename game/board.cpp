#pragma once
#include "board.h"
#include "tile.h"
#include "validation.h"
#include <QEventLoop>
#include <QGridLayout>
#include <QTimer>
#include <string>

Board::Board(QLabel* background = 0, const QSettings& settings_par = {}) :
	tile_size(background->width() / 9),
    settings(settings_par),
    side(settings.value("match_side").toBool()),  // true for user on white side
	valid(new Validation(this)),
    //tiles{ { } },
    turn(true),  // true for white turn;
    from_tile{},  // always actualized in Tile::setPiece()
    white_king{},  // ditto
    black_king{},  // ditto
    last_move{},
	virtual_move{},
    //menu{},
    board_css(
		"Board{"
		"background-color: rgb(170, 170, 125);"
		"border: 1 solid black;"
		"border-radius: 14px;}"
		"Tile{"
		"border-radius: 7;}"
	),
	promo_css(
		"Tile{"
		"background-color: white;}:"
		"hover{"
		"background-color: rgb(170,85,127);}"
    ),
    last_promotion('e'),
    last_virtually_passed{}
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setMinimumSize(396, 396);
	setMaximumSize(396, 396);
	setStyleSheet(board_css);

	setToolTip("Think thoroughly");

	background->parentWidget()->layout()->replaceWidget(background, this);
	// replace ui board, by this class
	background->~QLabel();

	drawLetters(side);
	drawNumbers(side);
    drawTiles(side);
}

void Board::reactOnClick(Tile* tile) {
    if (!from_tile) {
		// if it's first click then pick the piece and
		// show valid moves
        if (turn == tile->piece_color &&
                (side == tile->piece_color ||
                 settings.value("game_regime").toString() != "friend_online") &&
                tile->piece_name != 'e') {
            from_tile = tile; // FIX: are you sure?
			valid->showValid(tile);
		}
	}
    else if (tile != from_tile.data() && turn == tile->piece_color && tile->piece_name != 'e') {
		// if the second click
		// is on the piece of same color then pick it instead
		valid->hideValid();
        from_tile = tile;
		valid->showValid(tile);
	}
	else if (valid->isValid(tile)) {
		// if it's the second click and move is valid
		// then move pieces
        valid->hideValid();
        halfMove(from_tile.data(), tile);
        valid->reactOnMove(from_tile->coord, tile->coord);
        from_tile = nullptr;
	}
	else
		emit newStatus(tatus::invalid_move);
}


void Board::drawLetters(bool side) {
	std::string letters = side ? "abcdefgh" : "hgfedcba";
	int width = tile_size, height = tile_size / 2,
		x = tile_size / 2, y = this->height() - height;
	for (char ch : letters) {
		QLabel* letter = new QLabel(this);
		letter->setGeometry(x, y, width, height);
		letter->setText(QString(ch));
		QFont font = letter->font();
		font.setPointSize(tile_size / 4);
		letter->setFont(font);
		letter->setAlignment(Qt::AlignCenter);
		x += width;
	}
	x = width / 2, y = 0;
	for (char ch : letters) {
		QLabel* letter = new QLabel(this);
		letter->setGeometry(x, y, width, height);
		letter->setText(QString(ch));
		QFont font = letter->font();
		font.setPointSize(tile_size / 4);
		letter->setFont(font);
		letter->setAlignment(Qt::AlignCenter);
		x += width;
	}
}

void Board::drawNumbers(bool side) {
	std::string digits = side ? "87654321" : "12345678";
	int width = tile_size / 2, height = tile_size,
		x = 0, y = tile_size / 2;
	for (char ch : digits) {
		QLabel* letter = new QLabel(this);
		letter->setGeometry(x, y, width, height);
		letter->setText(QString(ch));
		QFont font = letter->font();
		font.setPointSize(tile_size / 4);
		letter->setFont(font);
		letter->setAlignment(Qt::AlignCenter);
		y += height;
	}
	x = this->width() - width, y = height / 2;
	for (char ch : digits) {
		QLabel* letter = new QLabel(this);
		letter->setGeometry(x, y, width, height);
		letter->setText(QString(ch));
		QFont font = letter->font();
		font.setPointSize(tile_size / 4);
		letter->setFont(font);
		letter->setAlignment(Qt::AlignCenter);
		y += height;
	}
}

void Board::drawTiles(bool side)
{
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
            tiles[x][y].reset( new Tile(this, {x, y}, side) );
			// uses Board::tile_side bool side to coordinate itself on board, i.e. setGeometry()
            QObject::connect(tiles[x][y].data(), &Tile::tileClicked, this, &Board::reactOnClick);
		}
	}

	//black pawns
	for (int x = 0; x < 8; x++)
        tiles[x][6]->setPiece('P', 0);

	//white pawns
	for (int x = 0; x < 8; x++)
        tiles[x][1]->setPiece('P', 1);

    tiles[0][7]->setPiece('R', 0);
    tiles[1][7]->setPiece('N', 0);
    tiles[2][7]->setPiece('B', 0);
    tiles[3][7]->setPiece('Q', 0);
    tiles[4][7]->setPiece('K', 0);
    tiles[5][7]->setPiece('B', 0);
    tiles[6][7]->setPiece('N', 0);
    tiles[7][7]->setPiece('R', 0);

    tiles[0][0]->setPiece('R', 1);
    tiles[1][0]->setPiece('N', 1);
    tiles[2][0]->setPiece('B', 1);
    tiles[3][0]->setPiece('Q', 1);
    tiles[4][0]->setPiece('K', 1);
    tiles[5][0]->setPiece('B', 1);
    tiles[6][0]->setPiece('N', 1);
    tiles[7][0]->setPiece('R', 1);
}

void Board::openPromotion(Tile* from)
{
	QEventLoop loop;
	std::string pieces = "QNRB";
	scoord coord = from->coord;
	for (int i = 0; i < 4; i++, coord.y += turn ? -1 : 1) {
        menu[i].reset (new Tile(this, coord, side));
        menu[i]->setStyleSheet(promo_css);
        menu[i]->setPiece(pieces[i], turn);
        menu[i]->raise();
        menu[i]->show();
        connect(menu[i].data(), &Tile::tileClicked, this, &Board::promotePawn);
        connect(this, &Board::promotionEnd, &loop, &QEventLoop::quit);
	}
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
            tiles[x][y]->setEnabled(false);
	loop.exec();
}

void Board::saveMove(Tile* from, Tile* to, pove& move)
{
    move.first = from->toVirtu();
    move.second = to->toVirtu();
}

void Board::revertVirtualMove(pove& move)
{
	virtu from = move.first;
	virtu to = move.second;
    restoreTile(from);
    restoreTile(to);
    if (last_virtually_passed.tile != nullptr){
        restoreTile(last_virtually_passed);
    }
    move = {};
}

void Board::moveVirtually(Tile* from, Tile* to, pove& move)
{
    if (valid->canPassVirtually(from, to)){
        last_virtually_passed = tiles[to->coord.x][from->coord.y]->toVirtu();
        tiles[to->coord.x][from->coord.y]->piece_name = 'e';
    }
    else{
        last_virtually_passed.tile = nullptr;
    }
    saveMove(from, to, move);
    to->piece_color = from->piece_color;
    to->piece_name = from->piece_name;
    from->piece_name = 'e';
}

void Board::moveNormally(Tile* from, Tile* to)
{
    if (turn == side)
        valid->hideValid(); // FIX: should be called only if it was our turn
	saveMove(from, to, last_move); // should be used before moving
    to->setPiece(from->piece_name, from->piece_color);
    from->setPiece('e', 0);
}

void Board::castleKing(Tile* king, Tile* destination, Tile* rook)
{
	moveNormally(king, destination);
	int k = destination->coord.x - king->coord.x > 0 ? -1 : 1;
	// the rook is always on the left or right side of king after castling
	int x = destination->coord.x + k;
	int y = destination->coord.y;
    moveNormally(rook, tiles[x][y].data());
}

void Board::passPawn(Tile* from, Tile* to)
{
	moveNormally(from, to);
    tiles[to->coord.x][from->coord.y]->setPiece('e', 0);
}

void Board::restoreTile(virtu saved)
{
    saved.tile->piece_color = saved.color;
    saved.tile->piece_name = saved.name;
}

void Board::promotePawn(Tile* tile)
{
    last_move.second.tile->setPiece(tile->piece_name, tile->piece_color);
    last_promotion = tile->piece_name;
	for (int i = 0; i < 4; i++) {
		menu[i]->~Tile();
	}
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			tiles[x][y]->setEnabled(true);
	emit promotionEnd();
}

void Board::halfMove(scoord from, scoord to)
{
	halfMove(valid->theTile(from), valid->theTile(to));
}

void Board::halfMove(Tile* from, Tile* to)
{
    char promotion_type = 'e';
	tatus emit_status = tatus::just_new_turn;
	if (valid->differentColor(to->coord))
        emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;

	if (Tile* rook; valid->canCastle(from, to, &rook)) {
		castleKing(from, to, rook);
		emit_status = tatus::castling;
	}
	else if (valid->canPass(from, to)) {
		passPawn(from, to);
        emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;
	}
	else
		moveNormally(from, to);

	if (valid->canPromote(to, to)) {
        if (turn == side){
            openPromotion(to);  // waits until the signal from a tile received
            promotion_type = last_promotion;
        }
		emit_status = tatus::promotion;
	}

    if (turn == side)
        emit moveMade(from->coord, to->coord, promotion_type);

	turn = !turn;
	if (valid->inCheck(turn))
		if (valid->inStalemate(turn))  // check + stalemate == checkmate
            emit theEnd(turn == side ? endnum::opponent_wins : endnum::user_wins);
		else
            emit newStatus(turn == side ? tatus::check_to_user : tatus::check_to_opponent);
	else if (valid->inStalemate(turn))
		emit theEnd(endnum::draw_by_stalemate);
	else
		emit newStatus(emit_status);
}

//void Board::virtualHalfMove(Tile* from, Tile* to, char promo_ty_par)
//{ // just a prototype
//	if (Tile* rook; valid->canCastle(from, to, &rook)) {
//		//castleKingVirtually(from, to, rook);
//	}
//	else
//		moveVirtually(from, to);

//	if (valid->canPromote(to, to)) {
//        if (turn == side){
//            //openPromotion(to);  // waits until the signal from a tile received
//            last_promotion = promo_ty_par;
//            promotePawn(to);
//        }
//	}

//	turn = !turn;
//	valid->inCheck(turn);
//    valid->reactOnMove(from->coord, to->coord);
//}




