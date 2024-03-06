#pragma once
#include "validator.h"
#include "board.h"
#include <QDebug>
using namespace std;
using lambda = function<bool(scoord)>;
using checker = function<bool(scoord, bool&)>;

Validator::Validator(Board* mother_board) :
	VirtualValidator(),
	board(mother_board)
{}

// beginning of virtual methods
Tile& Validator::theTile(scoord coord)
{
	return *board->theTile(coord);
}

bool Validator::theTurn()
{
	return board->turn;
}

Tile& Validator::theWKing()
{
	return *board->white_king;
}

Tile& Validator::theBKing()
{
	return *board->black_king;
}

void Validator::moveVirtually(scoord from, scoord to, vove& saved_move) {
	board->moveVirtually(&theTile(from), &theTile(to), saved_move);
}
void Validator::revertVirtualMove(vove& saved_move)
{
	board->revertVirtualMove(saved_move);
}
const std::vector<halfmove>& Validator::theStory()
{
	return board->history;
}
// end of virtual methods

void Validator::showValid(scoord from)
{
	findValid(from);
	if (!valid_moves.empty()) {
		for (auto move : valid_moves)
			theTile(move).dyeValid();
		theTile(from).dyeSelected();
	}
}

void Validator::hideValid()
{
	QString game_regime = settings["game_regime"].toString();
	if (game_regime != "history") {
		if (board->from_tile != nullptr)
			board->from_tile->dyeNormal();
		else
			qDebug() << "You try to dyeNormal() from_tile that is nullptr";
		for (auto coord : valid_moves)
			theTile(coord).dyeNormal();
	}
	valid_moves.clear();
}