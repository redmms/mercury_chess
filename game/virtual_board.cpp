#pragma once
#include "virtual_board.h"
#include <QDebug>
using namespace std;

VirtualBoard::VirtualBoard(QObject* parent) :
	QObject(parent),
	valid(this),
	turn(true),  // true for white turn;
	from_tile{},  // always actualized in Tile::setPiece()
	white_king{},  // ditto
	black_king{},  // ditto
	last_promotion('e'),
	end_type(endnum::interrupt)
{
	setTiles();
}

void VirtualBoard::setTiles()
{
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			tiles[x][y] = VirtualTile({ x, y }, 'e', false, this);
		}
	}

	// black pawns
	for (int x = 0; x < 8; x++) {
		tiles[x][6].setPiece('P', 0);
		//black_piece_coords[x + 8] = {x, 6};
	}

	// white pawns
	for (int x = 0; x < 8; x++) {
		tiles[x][1].setPiece('P', 1);
	}

	// black pieces
	string pieces = "RNBQKBNR";
	for (int x = 0; x < 8; x++) {
		tiles[x][7].setPiece(pieces[x], 0);
		//black_piece_coords[x] = {x, 7};
	}

	// white pieces
	for (int x = 0; x < 8; x++) {
		tiles[x][0].setPiece(pieces[x], 1);
		//piece_coords[x + 8] = {x, 0};
	}
}

//void VirtualBoard::saveMoveNormally(VirtualTile from, VirtualTile to, vove& move)
//{
//	move = {from, to};
//}

void VirtualBoard::moveNormally(VirtualTile& from, VirtualTile& to)
{
	to.setPiece(from.piece_name, from.piece_color);
	from.setPiece('e', 0);
	valid.reactOnMove(from.coord, to.coord);
}

void VirtualBoard::revertMoveNormally(vove& move)
{
	VirtualTile from_tile = move.first;
	scoord from_coord = from_tile.coord;
	VirtualTile to_tile = move.second;
	scoord to_coord = to_tile.coord;
	restoreTile(theTile(from_coord), from_tile);
	restoreTile(theTile(to_coord), to_tile);
	move = {};
}

void VirtualBoard::castleKing(VirtualTile& king, VirtualTile& destination, VirtualTile& rook)
{
	moveNormally(king, destination);
	int k = destination.coord.x - king.coord.x > 0 ? -1 : 1;
	// because the rook is always on the left or right side of king after castling
	int x = destination.coord.x + k;
	int y = destination.coord.y;
	moveNormally(rook, tiles[x][y]);
}

void VirtualBoard::passPawn(VirtualTile& from, VirtualTile& to)
{
	moveNormally(from, to);
	tiles[to.coord.x][from.coord.y].setPiece('e', 0);
}

void VirtualBoard::promotePawn(VirtualTile& from, char into)
{
	from.setPiece(into, from.piece_color);
	last_promotion = into;
}

void VirtualBoard::promotePawn(scoord from, char into)
{
	promotePawn(theTile(from), into);
}

void VirtualBoard::restoreTile(VirtualTile& restored, VirtualTile saved)
{
	restored.setPiece(saved.piece_name, saved.piece_color);
}

void VirtualBoard::revertCastling(vove move)
{	
	VirtualTile& from = theTile(move.first);
	VirtualTile& to = theTile(move.second);
	valid.bringBack(from.coord, to.coord);
	revertMoveNormally(move);
	scoord rook;
	if (valid.canCastle(from.coord, to.coord, rook)) { // FIX: will not work, because has_moved[] in VirtualValidator valid wasn't updated back for the rook
		int k = to.coord.x - from.coord.x > 0 ? -1 : 1;
		int x = to.coord.x + k;
		int y = to.coord.y;
		vove rook_move = { theTile(rook), tiles[x][y] };
		revertMoveNormally(rook_move);
	}
	else {
		qWarning() << "valid.canCastle(rook) returned false though it definetely should has returned true";
	}
	
}

void VirtualBoard::revertPass(vove move)
{
	revertMoveNormally(move);
	VirtualTile from = move.first;
	VirtualTile to = move.second;
	tiles[to.coord.x][from.coord.y].setPiece('P', !from.piece_color);
}

void VirtualBoard::revertPromotion(vove move)
{
	//char piece_name = move.first.piece_name;
	//bool piece_color = move.first.piece_color;
	revertMoveNormally(move);
	//theTile(move.first).setPiece(piece_name, piece_color);
}

void VirtualBoard::revertHalfmove(halfmove hmove)
{
	if (hmove.castling) {
		revertCastling(hmove.move);
	}
	else if (hmove.pass) {
		revertPass(hmove.move);
	}
	else if (hmove.promo != 'e') {
		revertPromotion(hmove.move);
	}
	else {
		revertMoveNormally(hmove.move);
	}
}

void VirtualBoard::revertCurrentMove()
{
	revertHalfmove(history[current_move]);
	current_move--;
}

void VirtualBoard::revertLastMove()
{
	if (!history.empty()) {
		revertHalfmove(history.back());
		history.pop_back();		
	}
}

void VirtualBoard::doCurrentMove()
{	
	halfmove hmove = history[current_move];
	VirtualTile from = hmove.move.first;
	VirtualTile to = hmove.move.second;
	char promo = hmove.promo;
	halfMove(from, to, promo);
	current_move++;
}

VirtualTile& VirtualBoard::theTile(scoord coord)
{
	return tiles[coord.x][coord.y];
}

VirtualTile& VirtualBoard::theTile(VirtualTile tile)
{
	return theTile(tile.coord);
}

void VirtualBoard::halfMove(scoord from, scoord to, char promo)
{
	halfMove(theTile(from), theTile(to), promo);
}

void VirtualBoard::halfMove(VirtualTile& from, VirtualTile& to, char promo)
{
	//valid.valid_moves.clear();
	halfmove last_move;
	last_move.move = {from, to};
	scoord rook;
	if (valid.canCastle(from.coord, to.coord, rook)) {
		castleKing(from, to, theTile(rook));
		last_move.castling = true;
	}
	else if (valid.canPass(from.coord, to.coord)) {
		passPawn(from, to);
		last_move.pass = true;
	}
	else {
		moveNormally(from, to);
	}
	if (valid.canPromote(to.coord, to.coord)) {
		promotePawn(to, promo);
		last_move.promo = promo;
	}
	last_move.turn = turn;
	history.push_back(last_move);
	turn = !turn;
	if (valid.inCheck(turn))
		if (valid.inStalemate(turn))  // check + stalemate == checkmate
			emit theEnd(turn == side ? endnum::opponent_wins : endnum::user_wins);
	else if (valid.inStalemate(turn))
		emit theEnd(endnum::draw_by_stalemate);
}
