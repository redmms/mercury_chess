#pragma once
#include "virtual_board.h"
#include <QDebug>
#include <cctype>
#include <iostream>
using namespace std;

VirtualBoard::VirtualBoard() :
	vvalid(this),
	from_coord{},  // always actualized in Tile::setPiece()
	white_king{},  // ditto
	black_king{},  // ditto
	//turn(true),  // true for white turn;
	side(settings["match_side"].toBool()),
	current_move(0),
	end_type(endnum::interrupt)
{	
	initTiles();
	setTiles();
}

void VirtualBoard::saveMoveNormally(scoord from, scoord to, vove& move)
{
	move = {*theTile(from), *theTile(to)};
	// FIX: will it call VirtualBoard::theTile() or Board::theTile() ?
}

void VirtualBoard::moveNormally(scoord from, scoord to, bool virtually)
{	
	auto from_tile = overTile(from, virtually);
	auto to_tile = overTile(to, virtually);
	to_tile->setPiece(from_tile->piece_name, from_tile->piece_color);
	from_tile->setPiece('e', 0);
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
	auto opp_to_tile = overTile({ to.x, from.y }, virtually);
	moveNormally(from, to, virtually);
	opp_to_tile->setPiece('e', 0);
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
	if (vvalid.inCheck(turn)){
		if (vvalid.inStalemate(turn)) {  // check + stalemate == checkmate
			end_type = turn == side ? endnum::opponent_wins : endnum::user_wins;
		}
	}
	else if (vvalid.inStalemate(turn)) {
		end_type = endnum::draw_by_stalemate;
	}
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
		promotePawn(to, promo); // FIX: could cause problems in call of moveVirtually()
		saved.promo = promo;
	}
	saved.turn = turn;
	turn = !turn;
	if (vvalid.inCheck(turn)) {
		if (vvalid.inStalemate(turn)) {
			end_type = turn == side ? endnum::opponent_wins : endnum::user_wins;
		}
	}
	else if (vvalid.fastInStalemate(turn)) {
		end_type = endnum::draw_by_stalemate;
	}
	cout << "After:\n"; // FIX: don't forget to delete
	cout << toStr();
}

void VirtualBoard::restoreTile(const VirtualTile& saved, bool virtually)
{
	auto tile = overTile(saved.coord, virtually);
	tile->setPiece(saved.piece_name, saved.piece_color);
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
	auto corner_tile = overTile(rook_corner, virtually);
	auto rook_tile = overTile(rook, virtually);
	corner_tile->setPiece('R', rook_tile->piece_color);
	rook_tile->setPiece('e', 0);
}

void VirtualBoard::revertPass(vove move, bool virtually)
{
	revertMoveNormally(move, virtually);
	scoord from = move.first.coord;
	scoord to = move.second.coord;
	auto opp_to_tile = overTile({ to.x, from.y }, virtually);
	opp_to_tile->setPiece('P', !move.first.piece_color);
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

VirtualTile* VirtualBoard::overTile(scoord coord, bool virtually)
{
	return virtually ? (VirtualTile*) theTile(coord) : theTile(coord);
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
	auto pawn_tile = overTile(from, virtually);
	pawn_tile->setPiece(into, pawn_tile->piece_color);
}
// end of virtual methods