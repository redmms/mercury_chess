#pragma once
#include "validator.h"
#include "board.h"
#include <QDebug>
using namespace std;
using lambda = function<bool(scoord)>;
using checker = function<bool(scoord, bool&)>;

Validator::Validator(Board* mother_board) :
    VirtualValidator(mother_board),
    board(mother_board)
{}

Tile* Validator::theTile(scoord coord)
{
    return board->theTile(coord);
}

void Validator::showValid(scoord from)
{
    findValid(from);
    if (!valid_moves.empty()) {
        for (auto move : valid_moves)
            theTile(move)->dyeValid();
        theTile(from)->dyeSelected();
    }
}

void Validator::hideValid()
{
    QString game_regime = settings["game_regime"].toString();
    if (game_regime != "history") {
        scoord from = board->from_coord;
        if (from != scoord{-1, -1})
            theTile(from)->dyeNormal();
        else
            qDebug() << "You try to dyeNormal() from_coord that is nullptr";
        for (auto move : valid_moves)
            theTile(move)->dyeNormal();
    }
    valid_moves.clear();
}

bool Validator::inStalemate(bool color)
{
    movable_pieces.clear();
    set<scoord> temp_valid_moves;
    bool stalemate = true;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            scoord coord{ x, y };
            if (occupied(coord) && !differentColor(coord)) {
                findValid(coord, temp_valid_moves); // should fastFindValid without temp_moves parameter
                if (!temp_valid_moves.empty()) {
                    temp_valid_moves.clear();
                    movable_pieces.insert(coord);
                    stalemate = false;
                }
            }
        }
    }
    return stalemate;
}