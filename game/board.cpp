#pragma once
#include "board.h"
#include "../app/mainwindow.h"
#include "tile.h"
#include "virtual_tile.h"
#include "../archiver/archiver.h"
#include <QEventLoop>
#include <QLayout>
#include <QDebug>
import finestream;
using namespace std;

Board::Board(MainWindow* parent_, QLabel* background_) :
	QLabel(parent_),
	//VirtualBoard(this),
	mainwindow(parent_),
	tile_size(background_->width() / 9),
	valid(this),
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
    )
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setMinimumSize(396, 396);
	setMaximumSize(396, 396);
	setStyleSheet(board_css);

	setToolTip("Think thoroughly");

	background_->parentWidget()->layout()->replaceWidget(background_, this);
    // replaces ui board by this class

	drawLetters();
	drawNumbers();
	initTiles();
    setTiles();
}

Board::~Board()
{
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			delete tiles[x][y];
		}
	}
}

void Board::initLetter(int x, int y, int width, int height, QString ch)
{
	QLabel* letter = new QLabel(this);
	letter->setGeometry(x, y, width, height);
	letter->setText(ch);
	QFont font = letter->font();
	font.setPointSize(tile_size / 4);
	letter->setFont(font);
	letter->setAlignment(Qt::AlignCenter);
}

void Board::drawLetters() {
	string letters = side ? "abcdefgh" : "hgfedcba";
	int width = tile_size, height = tile_size / 2,
		x = tile_size / 2, y1 = this->height() - height, y2 = 0;
	for (char ch : letters) {
		initLetter(x, y1, width, height, QString(ch));
		initLetter(x, y2, width, height, QString(ch));
		x += width;
	}
}

void Board::drawNumbers() {
	string digits = side ? "87654321" : "12345678";
	int width = tile_size / 2, height = tile_size,
		x1 = 0, x2 = this->width() - width, y = tile_size / 2;
	for (char ch : digits) {
		initLetter(x1, y, width, height, QString(ch));
		initLetter(x2, y, width, height, QString(ch));
		y += height;
	}
}

void Board::reactOnClick(Tile* tile) {
	scoord coord = tile->coord;
	QString game_regime = settings["game_regime"].toString();
	if (game_regime == "history") {
		return;
	}
    else if (from_coord == scoord{-1, -1}) {
		// if it's first click then pick the piece and
		// show valid moves
        if (turn == tile->piece_color &&
            (side == tile->piece_color ||
            game_regime != "friend_online") &&
            tile->piece_name != 'e') {
				from_coord = coord; // FIX: are you sure?
				valid.showValid(coord);
		}
	}
    else if (coord != from_coord 
			&& turn == tile->piece_color 
			&& tile->piece_name != 'e') {
			// if the second click
			// is on the piece of same color then pick it instead
				valid.hideValid();
				from_coord = coord;
				valid.showValid(coord);
	}
	else if (valid.isValid(coord)) {
		// if it's the second click and move is valid
		// then move pieces
		valid.hideValid();
		halfmove move;
        halfMove(from_coord, coord, 'e', move);
		history.push_back(move);
		//valid.hideValid(); // FIX: should go after halfMove
        from_coord = {-1, -1};
	}
	else
		emit newStatus(tatus::invalid_move);
}

char Board::openPromotion(scoord from)
{
	QEventLoop loop;
	string pieces = "QNRB";
	char promo;
	scoord menu_coord = from;
	for (int i = 0, k = turn ? -1 : 1; i < 4; i++, menu_coord.y += k) {
		menu[i] = (new Tile(this, menu_coord, side));
		menu[i]->setStyleSheet(promo_css);
		menu[i]->setPiece(pieces[i], turn);
		menu[i]->raise();
		menu[i]->show();
		QObject::connect(menu[i], &Tile::tileClicked, [&](Tile* into) {
			promo = into->piece_name;
			promotePawn(from, promo);
		});
		QObject::connect(this, &Board::promotionEnd, &loop, &QEventLoop::quit);
	}
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			tiles[x][y]->setEnabled(false);
	loop.exec();
	return promo;
}

void Board::initTiles()
{
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			tiles[x][y] = (new Tile(this, { x, y }, side));
			// uses Board::tile_side bool side to coordinate itself on board, i.e. setGeometry()
			QObject::connect(tiles[x][y], &Tile::tileClicked, this, &Board::reactOnClick);
		}
	}
}

Tile* Board::theTile(scoord coord)
{
	return Board::tiles[coord.x][coord.y];
}

void Board::promotePawn(scoord from, char into)
{
	theTile(from)->setPiece(into, theTile(from)->piece_color); // FIX: how halfMove() will know promotion type?
	QString game_regime = settings["game_regime"].toString();
	if (game_regime == "friend_offline" || game_regime == "friend_online" && turn == side) {
		for (int i = 0; i < 4; i++) {
			delete menu[i];
		}
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				tiles[x][y]->setEnabled(true);
	}
	emit promotionEnd();
}

void Board::halfMove(scoord from, scoord to, char promo)
{
	QString game_regime = settings["game_regime"].toString();

	tatus emit_status = tatus::just_new_turn;
	if (valid.differentColor(to))
		emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;

	scoord rook;
	if (valid.canCastle(from, to, rook)) {
		castleKing(from, to, rook);
		emit_status = tatus::castling;
	}
	else if (valid.canPass(from, to)) {
		passPawn(from, to);
		emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;
	}
	else
		moveNormally(from, to);

	char chosen_promo = 'e';
	if (game_regime == "history" && promo != 'e') {
		promotePawn(to, promo);
		emit_status = tatus::promotion;
	}
	else if ((game_regime == "friend_online" && turn == side || game_regime == "friend_offline") && valid.canPromote(to, to)) {
		chosen_promo = openPromotion(to);  // waits until the signal from a tile received
		emit_status = tatus::promotion;
	}

	if (turn == side)
		emit moveMade(from, to, promo != 'e' ? promo : chosen_promo);

	turn = !turn;
	if (valid.inCheck(turn))
		if (valid.inStalemate(turn))  // check + stalemate == checkmate
			emit theEnd(turn == side ? endnum::opponent_wins : endnum::user_wins);
		else
			emit newStatus(turn == side ? tatus::check_to_user : tatus::check_to_opponent);
	else if (valid.inStalemate(turn))
		emit theEnd(endnum::draw_by_stalemate);
	else
		emit newStatus(emit_status);

}

void Board::halfMove(scoord from, scoord to, char promo, halfmove& hmove)
{
	QString game_regime = settings["game_regime"].toString();

	saveMoveNormally(from, to, hmove.move);
	tatus emit_status = tatus::just_new_turn;
	if (valid.differentColor(to))
		emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;

	scoord rook;
	if (valid.canCastle(from, to, rook)) {
		castleKing(from, to, rook);
		hmove.castling = true;
		emit_status = tatus::castling;
	}
	else if (valid.canPass(from, to)) {
		passPawn(from, to);
		hmove.pass = true;
		emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;
	}
	else
		moveNormally(from, to);

	if (game_regime == "history" && promo != 'e') {
		promotePawn(to, promo);
		emit_status = tatus::promotion;
	}
	else if ((game_regime == "friend_online" && turn == side || game_regime == "friend_offline") && valid.canPromote(to, to)) {
		hmove.promo = openPromotion(to);  // waits until the signal from a tile received
		emit_status = tatus::promotion;
	}

	if (turn == side)
		emit moveMade(from, to, promo != 'e' ? promo : hmove.promo);

	hmove.turn = turn;
	history.push_back(hmove);

	turn = !turn;
	if (valid.inCheck(turn))
		if (valid.inStalemate(turn))  // check + stalemate == checkmate
			emit theEnd(turn == side ? endnum::opponent_wins : endnum::user_wins);
		else
			emit newStatus(turn == side ? tatus::check_to_user : tatus::check_to_opponent);
	else if (valid.inStalemate(turn))
		emit theEnd(endnum::draw_by_stalemate);
	else
		emit newStatus(emit_status);
}

void Board::halfMove(scoord from, scoord to, char promo, bitmove& bmove, halfmove& hmove)
{
	QString game_regime = settings["game_regime"].toString();

	// we do it before new move, but after the last one, which we convert
	// because at the moment of stalemate search we don't know what move we'll do -
	// we check stalemate right after the move - for the opponent's color
	if (game_regime == "friend_online" && turn != side) {
		valid.findValid(from);
	}
	bmove.move = Archiver::toMoveIdx(to, valid);
	valid.hideValid();
	if (history.empty()){ // first move
		if (turn != true) {
			qWarning() << "Board::history is empty but turn is not white as it should be for the first move";
		}
		valid.inStalemate(true);
		// should go afer findValid, because it cleares valid_moves
		// FIX: inStalemate() тоже меняет valid_moves во время поиска 
	} 
	bmove.piece = Archiver::toPieceIdx(from, valid);
	// FIX: but the problem is - we don't call inStalemate for the first move
	// and we need to call it externally. Or we don't?

	saveMoveNormally(from, to, hmove.move);
    //char promotion_type = 'e';
	tatus emit_status = tatus::just_new_turn;
	if (valid.differentColor(to))
        emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;

    scoord rook;
    if (valid.canCastle(from, to, rook)) {
		castleKing(from, to, rook);
		hmove.castling = true;
		emit_status = tatus::castling;
	}
	else if (valid.canPass(from, to)) {
		passPawn(from, to);
		hmove.pass = true;
        emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;
	}
	else
		moveNormally(from, to);

	if (game_regime == "history" && promo != 'e') {
		promotePawn(to, promo);
		emit_status = tatus::promotion;
	}
	else if ((game_regime == "friend_online" && turn == side || game_regime == "friend_offline") && valid.canPromote(to, to)) {
		hmove.promo = openPromotion(to);  // waits until the signal from a tile received
		bmove.promo = promo_by_char[hmove.promo];
		emit_status = tatus::promotion;
	}

    if (turn == side)
        emit moveMade(from, to, promo != 'e' ? promo : hmove.promo);

	hmove.turn = turn;
    history.push_back(hmove);
	bistory.push_back(bmove);

	turn = !turn;
	if (valid.inCheck(turn))
		if (valid.inStalemate(turn))  // check + stalemate == checkmate
            emit theEnd(turn == side ? endnum::opponent_wins : endnum::user_wins);
		else
            emit newStatus(turn == side ? tatus::check_to_user : tatus::check_to_opponent);
	else if (valid.inStalemate(turn))
		emit theEnd(endnum::draw_by_stalemate);
	else
		emit newStatus(emit_status);


		// bitmove.move should be updated during a halfmove, but bitmove.piece before it, 
		// even though they are separate values
}
