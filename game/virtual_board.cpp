#pragma once
#include "virtual_board.h"
#include "virtual_tile.h"
#include <QDebug>
#include <cctype>
#include <iostream>
using namespace std;

VirtualBoard::VirtualBoard() :
    valid(new VirtualValidator(this)),
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

VirtualTile* VirtualBoard::theTile(scoord coord)
{
    return VirtualBoard::tiles[coord.x][coord.y];
}

bool VirtualBoard::theTurn()
{
    return turn;
}

scoord VirtualBoard::wKing()
{
    return white_king;
}

scoord VirtualBoard::bKing()
{
    return black_king;
}

const std::vector<halfmove>& VirtualBoard::story()
{
    return history;
}

void VirtualBoard::saveMoveSimply(scoord from, scoord to, vove& move)
{
    move = {*theTile(from), *theTile(to)};
}

void VirtualBoard::moveSimply(scoord from, scoord to, bool virtually)
{    
    auto from_tile = theTile(from);
    auto to_tile = theTile(to);
    to_tile->setPiece(from_tile->piece_name, from_tile->piece_color, virtually);
    from_tile->setPiece('e', 0, virtually);
    valid->updateHasMoved(from, to);
}

void VirtualBoard::castleKing(scoord king, scoord destination, scoord rook, bool virtually)
{
    moveSimply(king, destination, virtually);
    int k = destination.x - king.x > 0 ? -1 : 1;
    int x = destination.x + k;
    int y = destination.y;
    scoord rook_destination{x, y};
    moveSimply(rook, rook_destination, virtually);
}

void VirtualBoard::passPawn(scoord from, scoord to, bool virtually)
{
    auto opp_to_tile = theTile({ to.x, from.y });
    moveSimply(from, to, virtually);
    opp_to_tile->setPiece('e', 0, virtually);
}

void VirtualBoard::halfMove(scoord from, scoord to, char promo)
{
    scoord rook;
    if (valid->canCastle(from, to, rook)) {
        castleKing(from, to, rook);
    }
    else if (valid->canPass(from, to)) {
        passPawn(from, to);
    }
    else {
        moveSimply(from, to);
    }
    if (valid->canPromote(to, to)) {
        promotePawn(to, promo);
    }
    turn = !turn;
}

void VirtualBoard::halfMove(scoord from, scoord to, char promo, halfmove& saved, bool virtually)
{    
    saveMoveSimply(from, to, saved.move);
    scoord rook;
    if (valid->canCastle(from, to, rook)) {
        castleKing(from, to, rook, virtually);
        saved.castling = true;
    }
    else if (valid->canPass(from, to)) {
        passPawn(from, to, virtually);
        saved.pass = true;
    }
    else {
        moveSimply(from, to, virtually);
    }
    if (valid->canPromote(to, to)) {
        promotePawn(to, promo, virtually);
        saved.promo = promo;
    }
    saved.turn = turn;
    saved.moved = arrToChar(valid->has_moved);
    saved.check = valid->check; 
    history.push_back(saved);
    
    if (!virtually) {
        turn = !turn;
        if (valid->inCheck(turn))
            if (valid->inStalemate(turn))  // check + stalemate == checkmate
                end_type = turn == side ? opponent_wins : user_wins;
            else if (valid->inStalemate(turn))
                end_type = draw_by_stalemate;
    }

    //cout << "\n";
    //cout << toStr(false);
    //cout << "\n";
}

void VirtualBoard::restoreTile(const VirtualTile& saved, bool virtually)
{
    auto tile = theTile(saved.coord);
    tile->setPiece(saved.piece_name, saved.piece_color, virtually);
}

void VirtualBoard::revertMoveSimply(vove move, bool virtually)
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
    revertMoveSimply(move, virtually);
    auto corner_tile = theTile(rook_corner);
    auto rook_tile = theTile(rook);
    corner_tile->setPiece('R', rook_tile->piece_color, virtually);
    rook_tile->setPiece('e', 0, virtually);
}

void VirtualBoard::revertPass(vove move, bool virtually)
{
    revertMoveSimply(move, virtually);
    scoord from = move.first.coord;
    scoord to = move.second.coord;
    auto opp_to_tile = theTile({ to.x, from.y });
    opp_to_tile->setPiece('P', !move.first.piece_color, virtually);
}

void VirtualBoard::revertPromotion(vove move, bool virtually)
{
    //char piece_name = move.first.piece_name;
    //bool piece_color = move.first.piece_color;
    revertMoveSimply(move, virtually);
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
        revertMoveSimply(hmove.move, virtually);
    }
    valid->check = hmove.check
    charToArr(hmove.moved, valid->hasMoved);
    if (!virtually) {
        turn = !turn;
        end_type = endnum::interrupt;
    }
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
void VirtualBoard::moveForward()
{
    if (0 <= current_move && current_move < history.size()) {
        halfmove hmove = history[current_move];
        scoord from = hmove.move.first.coord;
        scoord to = hmove.move.second.coord;
        char promo = hmove.promo;
        halfMove(from, to, promo);
        current_move++;    
    }
}

void VirtualBoard::moveBack()
{   
    if (0 < current_move && current_move < history.size()) {
        current_move--;
        revertHalfmove(history[current_move]);
    }
}

string VirtualBoard::toStr(bool stat)
{
    string view;
    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 8; x++) {
            auto tile = stat ? VirtualBoard::theTile({ x, y }) : theTile({ x, y });
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

void VirtualBoard::promotePawn(scoord from, char& into, bool virtually)
{    
    auto pawn_tile = theTile(from);
    pawn_tile->setPiece(into, pawn_tile->piece_color, virtually);
    valid->updateHasMoved(from, from);
}