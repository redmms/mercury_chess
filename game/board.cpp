#pragma once
#include "board.h"
#include "../app/mainwindow.h"
#include "tile.h"
#include "virtual_tile.h"
#include <QEventLoop>
#include <QLayout>
#include <QDebug>
#include <QLabel>
import finestream;
using namespace std;

Board::Board(QLabel* background, MainWindow* mainwindow_) :
	QLabel(mainwindow_),
	mainwindow(mainwindow_),
	tile_size(background->width() / 9),
    side(settings["match_side"].toBool()),  // true for user on white side
	valid(this),
    //tiles{ { } },
    turn(true),  // true for white turn;
    from_tile{},  // always actualized in Tile::setPiece()
    white_king{},  // ditto
    black_king{},  // ditto
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
    last_virtually_passed{},
	end_type(endnum::interrupt)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setMinimumSize(396, 396);
	setMaximumSize(396, 396);
	setStyleSheet(board_css);

	setToolTip("Think thoroughly");

	background->parentWidget()->layout()->replaceWidget(background, this);
    // replaces ui board, by this class

	drawLetters(side);
	drawNumbers(side);
    drawTiles(side);
}

Board::~Board()
{
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			delete tiles[x][y];
		}
	}

}

void Board::reactOnClick(Tile* tile) {
	if (settings["game_regime"].toString() == "history") {
		return;
	}
    else if (!from_tile) {
		// if it's first click then pick the piece and
		// show valid moves
        if (turn == tile->piece_color &&
                (side == tile->piece_color ||
                 settings["game_regime"].toString() != "friend_online") &&
                tile->piece_name != 'e') {
            from_tile = tile; // FIX: are you sure?
			valid.showValid(tile->coord);
		}
	}
    else if (tile != from_tile && turn == tile->piece_color && tile->piece_name != 'e') {
		// if the second click
		// is on the piece of same color then pick it instead
		valid.hideValid();
        from_tile = tile;
		valid.showValid(tile->coord);
	}
	else if (valid.isValid(tile->coord)) {
		// if it's the second click and move is valid
		// then move pieces
        halfMove(from_tile, tile);
		//valid.hideValid(); // FIX: should go after halfMove
        from_tile = nullptr;
	}
	else
		emit newStatus(tatus::invalid_move);
}


void Board::drawLetters(bool side) {
	string letters = side ? "abcdefgh" : "hgfedcba";
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
	string digits = side ? "87654321" : "12345678";
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
            tiles[x][y] = ( new Tile(this, {x, y}, side) );
			// uses Board::tile_side bool side to coordinate itself on board, i.e. setGeometry()
            QObject::connect(tiles[x][y], &Tile::tileClicked, this, &Board::reactOnClick);
		}
	}

//    pair<int, int> white_pieces_coords[16]{
//        {0, 0},
//        {1, 0},
//        {2, 0},
//        {3, 0},
//        {4, 0},
//        {5, 0},
//        {6, 0},
//        {7, 0},

//        {0, 1},
//        {1, 1},
//        {2, 1},
//        {3, 1},
//        {4, 1},
//        {5, 1},
//        {6, 1},
//        {7, 1}
//    };
//    pair<int, int> black_pieces_coords[16]{
//        {0, 7},
//        {1, 7},
//        {2, 7},
//        {3, 7},
//        {4, 7},
//        {5, 7},
//        {6, 7},
//        {7, 7},

//        {0, 6},
//        {1, 6},
//        {2, 6},
//        {3, 6},
//        {4, 6},
//        {5, 6},
//        {6, 6},
//        {7, 6}
//    };

	//black pawns
    for (int x = 0; x < 8; x++){
        tiles[x][6]->setPiece('P', 0);
        //black_piece_coords[x + 8] = {x, 6};
    }

	//white pawns
    for (int x = 0; x < 8; x++){
        tiles[x][1]->setPiece('P', 1);
    }

    // black pieces
    string pieces = "RNBQKBNR";
    for (int x = 0; x < 8; x++){
        tiles[x][7]->setPiece(pieces[x], 0);
       //black_piece_coords[x] = {x, 7};
    }

    // white pieces
    for (int x = 0; x < 8; x++){
        tiles[x][0]->setPiece(pieces[x], 1);
        //piece_coords[x + 8] = {x, 0};
    }
}

void Board::openPromotion(Tile* from)
{
	QEventLoop loop;
	string pieces = "QNRB";
	scoord coord = from->coord;
	for (int i = 0; i < 4; i++, coord.y += turn ? -1 : 1) {
        menu[i] = (new Tile(this, coord, side));
        menu[i]->setStyleSheet(promo_css);
        menu[i]->setPiece(pieces[i], turn);
        menu[i]->raise();
        menu[i]->show();
        connect(menu[i], &Tile::tileClicked, [&](Tile* into){
            promotePawn(from, into);
        });
        connect(this, &Board::promotionEnd, &loop, &QEventLoop::quit);
	}
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
            tiles[x][y]->setEnabled(false);
	loop.exec();
}

void Board::saveMoveNormally(Tile* from, Tile* to, vove& move)
{
    move.first = from->toVirtu();
    move.second = to->toVirtu();
}

void Board::revertVirtualMove(vove& move)
{
	VirtualTile from = move.first;
	VirtualTile to = move.second;
    restoreTile(from);
    restoreTile(to);
    if (last_virtually_passed.piece_name != 'e'){
        restoreTile(last_virtually_passed);
    }
    move = {};
}

void Board::moveVirtually(Tile* from, Tile* to, vove& move)
{
    if (valid.canPass(from->coord, to->coord)){
        last_virtually_passed = theTile({to->coord.x, from->coord.y})->toVirtu();
        theTile({to->coord.x, from->coord.y})->piece_name = 'e';
    }
    else{
        last_virtually_passed.piece_name = 'e';
    }
    saveMoveNormally(from, to, move);
    to->piece_color = from->piece_color;
    to->piece_name = from->piece_name;
    from->piece_name = 'e';
}

void Board::moveNormally(Tile* from, Tile* to)
{
//    halfmove last_move;
//    saveMoveNormally(from, to, last_move.move); // should be used before moving
//    history.push_back(last_move);
    to->setPiece(from->piece_name, from->piece_color);
    from->setPiece('e', 0);
	valid.reactOnMove(from->coord, to->coord);
}

void Board::castleKing(Tile* king, Tile* destination, Tile* rook)
{
	moveNormally(king, destination);
	int k = destination->coord.x - king->coord.x > 0 ? -1 : 1;
	// the rook is always on the left or right side of king after castling
	int x = destination->coord.x + k;
	int y = destination->coord.y;
    moveNormally(rook, tiles[x][y]);
}

void Board::passPawn(Tile* from, Tile* to)
{
	moveNormally(from, to);
    tiles[to->coord.x][from->coord.y]->setPiece('e', 0);
}

void Board::restoreTile(VirtualTile saved)
{
    theTile(saved)->setPiece(saved.piece_name, saved.piece_color);
}

bitmove Board::toBitmove(halfmove hmove)
{
	Tile* from = theTile(hmove.move.first);
	Tile* to = theTile(hmove.move.second);
	return { toPieceIdx(from), toMoveIdx(to), promo_by_char[hmove.promo] };
}

bitremedy Board::toPieceIdx(Tile* from) {
	unsigned char piece_idx = distance(valid.movable_pieces.begin(), valid.movable_pieces.find(from->coord));
	int bitsn = fsm::MinBits(valid.movable_pieces.size() - 1);
	return { piece_idx, fsm::MinBits(valid.movable_pieces.size() - 1), false };
}
bitremedy Board::toMoveIdx(Tile* to) {
	unsigned char move_idx = distance(valid.valid_moves.begin(), valid.valid_moves.find(to->coord));
	int sz = valid.valid_moves.size() - 1;
	int bitsn = fsm::MinBits(sz);
	return { move_idx, fsm::MinBits(valid.valid_moves.size() - 1), false };
}

Tile* Board::theTile(scoord coord)
{
	return tiles[coord.x][coord.y];
}

Tile* Board::theTile(VirtualTile tile)
{
	return theTile(tile.coord);
}

//int Board::idx(scoord coord)
//{
//    for (int i = 0; i < piece_coords.size(); i++){
//        if (piece_coords[i] == coord)
//            return i;
//    }
//}
// 
//bitmove Board::toBitmove(halfmove hmove)
//{
//	bitmove bmove;
//	Tile* from = hmove.move.first.tile;
//	Tile* to = hmove.move.second.tile;
//	unsigned char piece_idx = distance(valid.movable_pieces.begin(), valid.movable_pieces.find(from));
//	piece_idx--;
//	unsigned char move_idx = distance(valid.valid_moves.begin(), valid.valid_moves.find(to));
//	move_idx--;
//	bmove.piece = bitremedy(piece_idx);
//	bmove.move = bitremedy(move_idx);
//	bmove.promo = promo_by_char[hmove.promo];
//	return bmove;
//}

void Board::promotePawn(Tile* from, Tile* into)
{
    from->setPiece(into->piece_name, into->piece_color);
    last_promotion = into->piece_name;
	for (int i = 0; i < 4; i++) {
		delete menu[i];
	}
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			tiles[x][y]->setEnabled(true);
	emit promotionEnd();
}
void Board::promotePawn(Tile* from, char into)
{
	from->setPiece(into, from->piece_color);
	last_promotion = into;
	if (settings["game_regime"].toString() != "history") {
		for (int i = 0; i < 4; i++) {
			delete menu[i];
		}
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				tiles[x][y]->setEnabled(true);
	}
	emit promotionEnd();
}

void Board::promotePawn(scoord from, char into)
{
	promotePawn(theTile(from), into);
}

void Board::halfMove(scoord from, scoord to)
{
	halfMove(theTile(from), theTile(to));
}

void Board::halfMove(Tile* from, Tile* to)
{
	QString game_regime = settings["game_regime"].toString();

	// we do it before new move, but after the last one, which we convert
	// because at the moment of stalemate search we don't know what move we'll do -
	// we check stalemate right after the move - for the opponent's color
	bitmove bmove;
	if (game_regime == "friend_online" && turn != side) {
		valid.findValid(from->coord);
	}
	bmove.move = toMoveIdx(to);
	valid.hideValid();
	if (history.empty()){ // first move
		if (turn != true) {
			qWarning() << "Board::history is empty but turn is not white as it should be for the first move";
		}
		valid.inStalemate(true);
		// should go afer findValid, because it cleares valid_move
		// FIX: inStalemate() тоже меняет valid_moves во время поиска 
	} 
	bmove.piece = toPieceIdx(from);
	// FIX: but the problem is - we don't call inStalemate for the first move
	// and we need to call it externally. Or we don't?

    halfmove last_move;
    last_move.move = {from->toVirtu(), to->toVirtu()};
    char promotion_type = 'e';
	tatus emit_status = tatus::just_new_turn;
	if (valid.differentColor(to->coord))
        emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;

    scoord rook;
    if (valid.canCastle(from->coord, to->coord, rook)) {
		castleKing(from, to, theTile(rook));
        last_move.castling = true;
		emit_status = tatus::castling;
	}
	else if (valid.canPass(from->coord, to->coord)) {
		passPawn(from, to);
        last_move.pass = true;
        emit_status = turn == side ? tatus::opponent_piece_eaten : tatus::user_piece_eaten;
	}
	else
		moveNormally(from, to);

	if (valid.canPromote(to->coord, to->coord)) {
        if ((game_regime == "friend_online" && turn == side) || game_regime == "friend_offline"){
            openPromotion(to);  // waits until the signal from a tile received
            last_move.promo = last_promotion;
            promotion_type = last_promotion;
			bmove.promo = promo_by_char[last_promotion];
        }
		else if (game_regime == "history") {
			promotePawn(to, last_promotion);
		}
		emit_status = tatus::promotion;
	}

    if (turn == side)
        emit moveMade(from->coord, to->coord, promotion_type);

    last_move.turn = turn;
    history.push_back(last_move);
	if (game_regime != "history")
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


		// bitmove.move надо обновлять во время хода, а bitmove.piece перед ходом, 
		// хоть и идут они по отдельности
		//emit needBoardUpdate();
}

//void Board::virtualHalfMove(Tile* from, Tile* to, char promnum_par)
//{ // just a prototype
//	if (Tile* rook; valid.canCastle(from, to, &rook)) {
//		//castleKingVirtually(from, to, rook);
//	}
//	else
//		moveVirtually(from, to);

//	if (valid.canPromote(to, to)) {
//        if (turn == side){
//            //openPromotion(to);  // waits until the signal from a tile received
//            last_promotion = promnum_par;
//            promotePawn(to);
//        }
//	}

//	turn = !turn;
//	valid.inCheck(turn);
//    valid.reactOnMove(from->coord, to->coord);
//}




