#pragma once
#include "virtual_board.h"
#include <QDebug>
#include <cctype>
#include <iostream>
using namespace std;

VirtualBoard::VirtualBoard() :
	vvalid(this),
	from_coord{-1, -1},  // always actualized in Tile::setPiece()
	white_king{4, 0},  // ditto
	black_king{4, 7},  // ditto
	turn(true),  // true for white turn;
	side(settings["match_side"].toBool()),
	current_move(0),
	end_type(endnum::interrupt)
{	
	initTiles();
	setTiles();
}

VirtualBoard::VirtualBoard(VirtualBoard& copy) :
	vvalid(this),
	from_coord{copy.from_coord},  // always actualized in Tile::setPiece()
	white_king{copy.white_king},  // ditto
	black_king{copy.black_king},  // ditto
	turn(copy.turn),  // true for white turn;
	side(copy.side),
	current_move(copy.current_move),
	end_type(copy.end_type)
{
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			tiles[x][y] = (VirtualTile*) (copy.tiles[x][y]);
		}
	}
}

VirtualBoard::~VirtualBoard()
{
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			delete VirtualBoard::tiles[x][y];
		}
	}
}

void VirtualBoard::saveMoveNormally(scoord from, scoord to, vove& move)
{
	move = {*theTile(from), *theTile(to)};
}

void VirtualBoard::moveNormally(scoord from, scoord to, bool virtually)
{	
	auto from_tile = theTile(from);
	auto to_tile = theTile(to);
	to_tile->setPiece(from_tile->piece_name, from_tile->piece_color, virtually);
	from_tile->setPiece('e', 0, virtually);
	vvalid.reactOnMove(from, to);
}

void VirtualBoard::castleKing(scoord king, scoord destination, scoord rook, bool virtually)
{
	moveNormally(king, destination, virtually);
	int k = destination.x - king.x > 0 ? -1 : 1;
	int x = destination.x + k;
	int y = destination.y;
	scoord rook_destination{x, y};
	moveNormally(rook, rook_destination, virtually);
}

void VirtualBoard::passPawn(scoord from, scoord to, bool virtually)
{
	auto opp_to_tile = theTile({ to.x, from.y });
	moveNormally(from, to, virtually);
	opp_to_tile->setPiece('e', 0, virtually);
}

void VirtualBoard::halfMove(scoord from, scoord to, char promo, endnum& end_type)
{
	scoord rook;
	if (vvalid.canCastle(from, to, rook)) {
		castleKing(from, to, rook);
	}
	else if (vvalid.canPass(from, to)) {
		passPawn(from, to);
	}
	else {
		moveNormally(from, to);
	}
	if (vvalid.canPromote(to, to)) {
		promotePawn(to, promo);
	}
	turn = !turn;
}

void VirtualBoard::halfMove(scoord from, scoord to, char promo, endnum& end_type, halfmove& saved, bool virtually)
{
	cout << "Before:\n"; // FIX: don't forget to delete
	cout << toStr();

	saveMoveNormally(from, to, saved.move); // FIX: could cause problems in call of moveVirtually()
	scoord rook;
	if (vvalid.canCastle(from, to, rook)) {
		castleKing(from, to, rook, virtually);
		saved.castling = true;
	}
	else if (vvalid.canPass(from, to)) {
		passPawn(from, to, virtually);
		saved.pass = true;
	}
	else {
		moveNormally(from, to, virtually);
	}
	if (vvalid.canPromote(to, to)) {
		promotePawn(to, promo, virtually); // FIX: could cause problems in call of moveVirtually()
		saved.promo = promo;
	}
	saved.turn = turn;
	turn = !turn;
	cout << "After:\n"; // FIX: don't forget to delete
	cout << toStr();
}

void VirtualBoard::restoreTile(const VirtualTile& saved, bool virtually)
{
	auto tile = theTile(saved.coord);
	tile->setPiece(saved.piece_name, saved.piece_color, virtually);
}

void VirtualBoard::revertMoveNormally(vove move, bool virtually)
{
	restoreTile(move.first, virtually);
	restoreTile(move.second, virtually);
}

void VirtualBoard::revertCastling(vove move, bool virtually)
{	
	scoord from = move.first.coord;
	scoord to = move.second.coord;
	int k = to.x - from.x > 0 ? -1 : 1;
	int x = to.x + k;
	int y = to.y;
	scoord rook{x, y};
	scoord rook_corner{x + k*3, y};
	vvalid.bringBack(from, to);
	revertMoveNormally(move, virtually);
	vvalid.bringBack(rook_corner, rook);
	auto corner_tile = theTile(rook_corner);
	auto rook_tile = theTile(rook);
	corner_tile->setPiece('R', rook_tile->piece_color, virtually);
	rook_tile->setPiece('e', 0, virtually);
}

void VirtualBoard::revertPass(vove move, bool virtually)
{
	revertMoveNormally(move, virtually);
	scoord from = move.first.coord;
	scoord to = move.second.coord;
	auto opp_to_tile = theTile({ to.x, from.y });
	opp_to_tile->setPiece('P', !move.first.piece_color, virtually);
}

void VirtualBoard::revertPromotion(vove move, bool virtually)
{
	//char piece_name = move.first.piece_name;
	//bool piece_color = move.first.piece_color;
	revertMoveNormally(move, virtually);
	//theTile(move.first).setPiece(piece_name, piece_color);
}

void VirtualBoard::revertHalfmove(halfmove hmove, bool virtually)
{
	if (hmove.castling) {
		revertCastling(hmove.move, virtually);
	}
	else if (hmove.pass) {
		revertPass(hmove.move, virtually);
	}
	else if (hmove.promo != 'e') {
		revertPromotion(hmove.move, virtually);
	}
	else {
		revertMoveNormally(hmove.move, virtually);
	}
	turn = !turn;
	end_type = endnum::interrupt;
}

void VirtualBoard::setTiles()
{
	// black pawns
	for (int x = 0; x < 8; x++) {
		theTile({x, 6})->setPiece('P', 0);
	}
	// white pawns
	for (int x = 0; x < 8; x++) {
		theTile({x, 1})->setPiece('P', 1);
	}
	// black pieces
	string pieces = "RNBQKBNR";
	for (int x = 0; x < 8; x++) {
		theTile({x, 7})->setPiece(pieces[x], 0);
	}
	// white pieces
	for (int x = 0; x < 8; x++) {
		theTile({x, 0})->setPiece(pieces[x], 1);
	}
}

// beggining of virtual methods
void VirtualBoard::doCurrentMove()
{
	halfmove hmove = history[current_move];
	scoord from = hmove.move.first.coord;
	scoord to = hmove.move.second.coord;
	char promo = hmove.promo;
	endnum end;
	halfMove(from, to, promo, end);
	current_move++;
}

void VirtualBoard::revertCurrentMove()
{
	revertHalfmove(history[current_move]);
	current_move--;
}

VirtualTile* VirtualBoard::theTile(scoord coord)
{
	return VirtualBoard::tiles[coord.x][coord.y];
}

string VirtualBoard::toStr()
{
	string view;
	for (int y = 7; y >= 0; y--) {
		for (int x = 0; x < 8; x++) {
			auto tile = theTile({ x, y });
			view.push_back(tile->piece_color ? tile->piece_name : tolower(tile->piece_name));
		}
		view.push_back('\n');
	}
	return view;
}

void VirtualBoard::initTiles()
{
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			tiles[x][y] = new VirtualTile({ x, y }, 'e', false, this);
		}
	}
}

void VirtualBoard::promotePawn(scoord from, char into, bool virtually)
{	
	auto pawn_tile = theTile(from);
	pawn_tile->setPiece(into, pawn_tile->piece_color, virtually);
}
// end of virtual methods