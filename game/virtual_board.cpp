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

void VirtualBoard::saveMoveNormally(scoord from, scoord to, vove& move)
{
	move = {*theTile(from), *theTile(to)};
	// FIX: will it call VirtualBoard::theTile() or Board::theTile() ?
}

void VirtualBoard::moveNormally(scoord from, scoord to)
{
	auto from_tile = theTile(from);
	auto to_tile = theTile(to);
	to_tile->setPiece(from_tile->piece_name, from_tile->piece_color);
	from_tile->setPiece('e', 0);
	vvalid.reactOnMove(from, to);
}

void VirtualBoard::castleKing(scoord king, scoord destination, scoord rook)
{
	moveNormally(king, destination);
	int k = destination.x - king.x > 0 ? -1 : 1;
	int x = destination.x + k;
	int y = destination.y;
	scoord rook_destination{x, y};
	moveNormally(rook, rook_destination);
}

void VirtualBoard::passPawn(scoord from, scoord to)
{
	moveNormally(from, to);
	theTile({to.x, from.y})->setPiece('e', 0);
}

void VirtualBoard::halfMove(scoord from, scoord to, char promo, bool save_story)
{
	//valid.valid_moves.clear();
	halfmove last_move;
	saveMoveNormally(from, to, last_move.move);
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
	if (save_story) {
		history.push_back(last_move);
	}
	turn = !turn;
	if (valid.inCheck(turn))
		if (valid.inStalemate(turn))  // check + stalemate == checkmate
			emit theEnd(turn == side ? endnum::opponent_wins : endnum::user_wins);
		else if (valid.inStalemate(turn))
			emit theEnd(endnum::draw_by_stalemate);
}

void VirtualBoard::doCurrentMove()
{
	halfmove hmove = history[current_move];
	scoord from = hmove.move.first.coord;
	scoord to = hmove.move.second.coord;
	char promo = hmove.promo;
	halfMove(from, to, promo, false);
	current_move++;
}

void VirtualBoard::restoreTile(const VirtualTile& saved)
{
	theTile(saved)->setPiece(saved.piece_name, saved.piece_color);
}

void VirtualBoard::revertMoveNormally(vove move)
{
	restoreTile(move.first);
	restoreTile(move.second);
}

void VirtualBoard::revertCastling(vove move)
{	
	scoord from = move.first.coord;
	scoord to = move.second.coord;
	int k = to.x - from.x > 0 ? -1 : 1;
	int x = to.x + k;
	int y = to.y;
	scoord rook{x, y};
	scoord rook_corner{x + k*3, y};
	vvalid.bringBack(from, to);
	revertMoveNormally(move);
	vvalid.bringBack(rook_corner, rook);
	theTile(rook_corner)->setPiece('R', theTile(rook)->piece_color);
	theTile(rook)->setPiece('e', 0);
}

void VirtualBoard::revertPass(vove move)
{
	revertMoveNormally(move);
	scoord from = move.first.coord;
	scoord to = move.second.coord;
	theTile({ to.x, from.y })->setPiece('P', !move.first.piece_color);
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

// beggining of virtual methods
VirtualTile* VirtualBoard::theTile(scoord coord)
{
	return &vtiles[coord.x][coord.y];
}

VirtualTile* VirtualBoard::theTile(VirtualTile tile)
{
	return theTile(tile.coord);
}

void VirtualBoard::setTiles()
{
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			vtiles[x][y] = VirtualTile({ x, y }, 'e', false, this);
		}
	}
	// black pawns
	for (int x = 0; x < 8; x++) {
		vtiles[x][6].setPiece('P', 0);
	}
	// white pawns
	for (int x = 0; x < 8; x++) {
		vtiles[x][1].setPiece('P', 1);
	}
	// black pieces
	string pieces = "RNBQKBNR";
	for (int x = 0; x < 8; x++) {
		vtiles[x][7].setPiece(pieces[x], 0);
	}
	// white pieces
	for (int x = 0; x < 8; x++) {
		vtiles[x][0].setPiece(pieces[x], 1);
	}
}

void VirtualBoard::promotePawn(scoord from, char into)
{
	theTile(from)->setPiece(into, theTile(from)->piece_color);
}
// end of virtual methods