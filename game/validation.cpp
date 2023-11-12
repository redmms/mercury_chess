#pragma once
#include "validation.h"
#include "board.h"
#include <algorithm>

using namespace std;
using pint = pair<int, int>;
using pintr = const pint&;
using lambda = function<bool(pintr)>;

void Validation::showValid(Tile * from)
{
    findValid(from);
    if (!m_valid_moves.empty()) {
        for (auto tile : m_valid_moves)
            tile->setStyleSheet("background: orange;");
        from->setStyleSheet("background: green;");
    }
}

void Validation::hideValid()
{
    m_board.m_from_tile->dyeNormal();
    for (auto tile : m_valid_moves)
        tile->dyeNormal();
    m_valid_moves.clear();
}

bool Validation::isValid(Tile* tile)
{
    return m_valid_moves.count(tile);
}

bool Validation::empty()
{
    return m_valid_moves.empty();
}

void Validation::findValid(Tile *from_tile)
{
    // NOTE: for the purpose of this code, we consider that m_board goes [x][y]
    // instead of [i][j] that is equal to [y][x];
    // and we also consider that white piece are on the 0 line and black on the 
    // 7 line
    const bool turn = m_board.m_white_turn;
    const char piece = from_tile->m_piece_name;
    const bool color = from_tile->m_white_piece;
    const pint king = color ? m_board.m_white_king->m_coord : m_board.m_black_king->m_coord;
    const pint from = from_tile->m_coord;
    int X, Y;
    bool first_evaluation = true, from_align_king, nothing_between;
    pint add;
    set <pint> potenial_moves;

    auto inBoard = [](pintr coord) -> bool {
        return coord.first >= 0 && coord.first < 8 && coord.second >= 0 && coord.second < 8;
        };
    auto occupied = [this](pintr coord) -> bool {
        return m_board[coord.first][coord.second]->m_piece_name != 'e';
        };
    auto differentColor = [this, color, occupied](pintr coord) -> bool {
        return m_board[coord.first][coord.second]->m_white_piece != color && occupied(coord);
        };
    auto pieceName = [this](pintr coord) -> char {
        return m_board[coord.first][coord.second]->m_piece_name;
        };
    auto addValid = [this](pintr coord) -> void {
        m_valid_moves.emplace(m_board[coord.first][coord.second]);  // FIX: should be insert or emplace?
        // because m_board[x][y] does already exist, it doesn't have to be created
        };
    auto runThrough = [&](pint coord, pintr add, lambda stop_cond, lambda borders_cond) -> bool {
        // here range is (from, to] or (from, to), depending on stop_cond
        coord.first += add.first, coord.second += add.second;
        while(borders_cond(coord)){
            if (stop_cond(coord))
                return true;
            coord.first += add.first, coord.second += add.second;
        }
        return false;
        };
    auto onDiagonals = [&](pintr coord, lambda stop_cond) -> bool {
        bool b1 = runThrough(coord, {  1,  1 }, stop_cond, inBoard),
             b2 = runThrough(coord, {  1, -1 }, stop_cond, inBoard),
             b3 = runThrough(coord, { -1,  1 }, stop_cond, inBoard),
             b4 = runThrough(coord, { -1, -1 }, stop_cond, inBoard);
        return b1 || b2 || b3 || b4;
        };
    auto onPerp = [&](pintr coord, lambda stop_cond) -> bool {
        bool b1 = runThrough(coord, { 0,  1  }, stop_cond, inBoard),
             b2 = runThrough(coord, { 0,  -1 }, stop_cond, inBoard),
             b3 = runThrough(coord, { 1,  0  }, stop_cond, inBoard),
             b4 = runThrough(coord, { -1, 0  }, stop_cond, inBoard);
        return b1 || b2 || b3 || b4;
        };
    auto findDirection = [&](pintr beg, pintr end) -> pint {
        int X = end.first - beg.first;
        int Y = end.second - beg.second;
        pint add{ X > 0 ? 1 : -1,  Y > 0 ? 1 : -1 };
        if (!Y) add.second = 0;
        else if (!X) add.first = 0;
        return add;
    };
    auto underAttack = [&](pintr coord) -> bool {
		return false;
		};    // FIX
    //auto afterFrom = [king, from](pintr coord) -> bool {
    //    int xmin = king.first, xmax = from.first;
    //    int ymin = king.second, ymax = from.second;
    //    if (xmin > xmax) swap(xmin, xmax);
    //    if (ymin > ymax) swap(ymin, ymax);
    //    return coord.first < xmin || coord.first > xmax || coord.second < ymin || coord.second > ymax;
    //};
    auto threatens = [&](pintr coord) {
        char name = pieceName(coord);
        return differentColor(coord) && (name == 'R' || name == 'B' || name == 'Q');
        //return "RBQ"sv.contains(pieceName(coord)); } // C++23
        // this line means that on the same line with king and the piece
        // there's some rook, bishop or queen.
        // We shouldn't be afraid of a pawn or knight, or other king. 
        };  
    auto anotherLine = [&from, &king](pintr coord){
    // checks that coord tile is not on the same line with king tile and from tile with the least operations possible
        return (from.first - coord.first) * (coord.second - king.second) != 
               (from.second - coord.second) * (coord.first - king.first);           
    };
    auto exposureKing = [&](pintr coord) -> bool {
        auto bordersBetween = [king, from](pintr coord) -> bool {
            return coord != king && coord != from;
            };
        if (first_evaluation){
            X = from.first - king.first,
            Y = from.second - king.second;
            from_align_king = abs(X) == abs(Y) || !X || !Y;
            add = findDirection(king, from);
            nothing_between = !runThrough(king, add, occupied, bordersBetween);
            first_evaluation = false;
        }
        if (piece == 'K')
            return underAttack(coord);
        // if the piece is King itself then we need to make another checking
        if (from_align_king && nothing_between && anotherLine(coord)) { // can add check that anotherLine with previous coord,
        // there's no need to check it for every tile in the line;
            auto bordersAfter = [&](pintr coor) {
                return inBoard(coor) && (!occupied(coor) || differentColor(coor)); 
                };
            return runThrough(from, add, threatens, bordersAfter);
        }
        return false;
        };
    auto canMoveTo = [&](pintr coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
               pieceName(coord) != 'K' && !exposureKing(coord);
    };
    auto pawnCanEat = [&](pintr coord) -> bool {
        return inBoard(coord) && differentColor(coord) &&
            pieceName(coord) != 'K' && !exposureKing(coord);
    };
    auto pawnCanMove = [&](pintr coord) -> bool {
        return inBoard(coord) && !occupied(coord) && !exposureKing(coord);
        };
    auto addMove = [&](pintr coord) -> bool {
        if (canMoveTo(coord)) {
            addValid(coord);
            if (occupied(coord))
                return true; // add move but stop, we can eat but cannot go next
		    return false; // continue
        }
        else
            return true; // break cycle
        };
    auto addPawnMoves = [&](pintr coord) -> void {
        int x = coord.first, y = coord.second;
        pint move;
        int k = turn ? 1 : -1;
        if (move = { x, y + 1 * k }; pawnCanMove(move)) {
            addValid(move);
            if (move = { x, y + 2 * k }; (turn ? from.second == 1 : from.second == 6) && pawnCanMove(move))  // FIX: add en passant
                addValid(move);
        }
        if (move = { x - 1, y + 1 * k }; pawnCanEat(move))
            addValid(move);
        if (move = { x + 1, y + 1 * k }; pawnCanEat(move))
            addValid(move);
        };
    auto kingPotential = [](pintr coord, set<pint>& coords) -> void {
        int x = coord.first, y = coord.second;
        coords =    { { x - 1, y + 1 }, { x, y + 1 }, { x + 1, y + 1 },
                      { x - 1, y },                   { x + 1, y },
                      { x - 1, y - 1 }, { x, y - 1 }, { x + 1, y - 1 } };
    };
    auto knightPotential = [](pintr coord, set<pint>& coords) -> void {
        int x = coord.first, y = coord.second;
        coords = { { x - 2, y + 1 }, { x - 1, y + 2 }, { x + 1, y + 2 }, { x + 2, y + 1 },
                   { x - 2, y - 1 }, { x - 1, y - 2 }, { x + 1, y - 2 }, { x + 2, y - 1 } };
        };
    switch (piece)
    {
    case 'P':  // pawn
        addPawnMoves(from);
    break;
    case 'N':  // knight
        knightPotential(from, potenial_moves);
        for (auto coord : potenial_moves)
            addMove(coord);
    break;
    case 'K':  // king
        kingPotential(from, potenial_moves);
        for (auto coord : potenial_moves)
            addMove(coord);
    break;
    case 'B':  // bishop
        onDiagonals(from, addMove);
    break;
    case 'R':  // rook
        onPerp(from, addMove);
    break;
    case 'Q':  // queen
        onPerp(from, addMove);
        onDiagonals(from, addMove);
    };
}
// FIX: we also need to add castling, transformation, en passant,
// and different chekings, at least three:
// stalemate, checkmate, check
// that will output the info to status bar
