#pragma once
#include "validation.h"
#include "board.h"
using namespace std;
using pint = pair<int, int>;
using lambda = function<bool(pint)>;

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
    bool turn = m_board.m_white_turn;
    char piece = from_tile->m_piece_name;
    bool color = from_tile->m_white_piece;
    const pint king = color ? m_board.m_white_king->m_coord : m_board.m_black_king->m_coord;
    const pint from = from_tile->m_coord;
    const int x = from.first, y = from.second;
    int k = from_tile->m_tile_num;
    set <pint> potenial_moves;

    auto inBoard = [](pint coord) -> bool {
        int x = coord.first, y = coord.second;
        return x >= 0 && x < 8 && y >= 0 && y < 8;
        };
    auto occupied = [this](pint coord) -> bool {
        int x = coord.first, y = coord.second;
        return m_board[x][y]->m_piece_name != 'e';
        };
    auto differentColor = [this, color, occupied](pint coord) -> bool {
        int x = coord.first, y = coord.second;
        return m_board[x][y]->m_white_piece != color && occupied(coord);
        };
    auto pieceName = [this](pint coord) -> char {
        int x = coord.first, y = coord.second;
        return m_board[x][y]->m_piece_name;
        };
    auto addValid = [this](pint coord) -> void {
        int x = coord.first, y = coord.second;
        m_valid_moves.emplace(m_board[x][y]);  // FIX: should be insert or emplace?
        // because m_board[x][y] does already exist, it doesn't have to be created
        };
    auto runThrough = [&](pint coord, pint add, lambda stop_cond, lambda borders_cond) -> bool {
        // here range is (from, to] or (from, to), depending on stop_cond
        do { 
            coord.first += add.first, coord.second += add.second;
            if (stop_cond(coord))
                return true;
        }
        while(borders_cond(coord));
        return false;
        };
    auto onDiagonals = [&](pint coord, lambda stop_cond) -> bool {
        bool b1 = runThrough(coord, {  1,  1 }, stop_cond, inBoard),
             b2 = runThrough(coord, {  1, -1 }, stop_cond, inBoard),
             b3 = runThrough(coord, { -1,  1 }, stop_cond, inBoard),
             b4 = runThrough(coord, { -1, -1 }, stop_cond, inBoard);
        return b1 || b2 || b3 || b4;
        };
    auto onPerp = [&](pint coord, lambda stop_cond) -> bool {
        bool b1 = runThrough(coord, { 0,  1  }, stop_cond, inBoard),
             b2 = runThrough(coord, { 0,  -1 }, stop_cond, inBoard),
             b3 = runThrough(coord, { 1,  0  }, stop_cond, inBoard),
             b4 = runThrough(coord, { -1, 0  }, stop_cond, inBoard);
        return b1 || b2 || b3 || b4;
        };
    auto inBetween = [&](pint coord, pint to, lambda stop_cond) -> bool {
    // here range is (coord; to)
        int X = to.first - coord.first;
        int Y = to.second - coord.second;
        auto borders_between = [coord, to](pint betw_coord){
            return betw_coord == coord || betw_coord == to;
            };
        if (!Y)  // horizontal line
            if (X > 0) // to the right
                return runThrough(coord, { 1, 0 }, stop_cond, borders_between);
            else  // to the left
                return runThrough(coord, { -1, 0 }, stop_cond, borders_between);
        else if (!X)  // vertical line
            if (Y > 0) // to the top
                return runThrough(coord, { 0, 1 }, stop_cond, borders_between);
            else  // to the bottom
                return runThrough(coord, { 0, -1 }, stop_cond, borders_between);
        else if (abs(X) == abs(Y))  // diagonal line
            if (X > 0 && Y > 0) // to the top right
                return runThrough(coord, { 1, 1 }, stop_cond, borders_between);
            else if (X > 0 && Y < 0) // to the bottom right
                return runThrough(coord, { 1, -1 }, stop_cond, borders_between);
            else if (X < 0 && Y > 0) // to the top left
                return runThrough(coord, { -1, 1 }, stop_cond, borders_between);
            else // to the bottom left  
                return runThrough(coord, { -1, -1 }, stop_cond, borders_between);
        };
    auto underAttack = [&](pint coord) -> bool {
		return false;
		};    // FIX
    auto exposureKing = [&](pint coord) -> bool {
        //return false;
        if (piece == 'K')
            return underAttack(coord);
        // if the piece is King itself then we need to make another checking

        int X = x - king.first;  // from.x
        int Y = y - king.second;  // from.y
        bool on_same_line = abs(X) == abs(Y) || !X || !Y;
        bool nothing_between = !inBetween(king, from, [occupied](pint coord) {
                return occupied(coord); }
             );
        
        if (on_same_line && nothing_between) {
            bool will_exposure = inBetween(from, coord, [pieceName](pint coord) {
                char name = pieceName(coord);
                return name == 'R' || name == 'B' || name == 'Q'; }
                //return "RBQ"sv.contains(pieceName(coord)); }
                // can be used only in C++23, just try it
                // this line means that on the same line with king and the piece
                // there's some rook, bishop or queen.
                // We shouldn't be afraid of a pawn or knight, or other king.   
                );
            return will_exposure;
        }
        else 
            return false;
        };
    auto canMoveTo = [&](pint coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
               pieceName(coord) != 'K' && !exposureKing(coord);
    };
    auto addMove = [&](pint coord) -> bool {
        if (canMoveTo(coord)) {
            addValid(coord);
            if (occupied(coord))
                return true; // add move but stop, we can eat but cannot go next
		    return false; // continue
        }
        else
            return true; // break cycle
        };
    auto kingPotential = [](pint coord, set<pint>& coords) -> void {
        int x = coord.first, y = coord.second;
        coords =    { { x - 1, y + 1 }, { x, y + 1 }, { x + 1, y + 1 },
                    { x - 1, y },                   { x + 1, y },
                    { x - 1, y - 1 }, { x, y - 1 }, { x + 1, y - 1 } };
    };
    auto pawnPotential = [turn](pint coord, set<pint>& coords) -> void {
        int x = coord.first, y = coord.second;
        if (turn)
            coords = { { x - 1, y + 1 }, { x, y + 1 }, { x + 1, y + 1 }, { x, y + 2 } };
        else
            coords = { { x - 1, y - 1 }, { x, y - 1 }, { x + 1, y - 1 }, { x, y - 2 } };
        };
    auto knightPotential = [](pint coord, set<pint>& coords) -> void {
        int x = coord.first, y = coord.second;
        coords = { { x - 2, y + 1 }, { x - 1, y + 2 }, { x + 1, y + 2 }, { x + 2, y + 1 },
                   { x - 2, y - 1 }, { x - 1, y - 2 }, { x + 1, y - 2 }, { x + 2, y - 1 } };
        };
    switch (piece)
    {
    case 'P':  // pawn
        pawnPotential(from, potenial_moves);
        for (auto coord : potenial_moves)
            addMove(coord);
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
