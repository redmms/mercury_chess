#pragma once
#include "validation.h"
#include "board.h"
using namespace std;
using pint = std::pair<int, int>;
using pintr = const pint&;
using lambda = function<bool(pint)>;
using func = bool (*)(pint);

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

bool Validation::inCheck(Tile* king)
{
    return underAttack(king->m_coord);
}

bool Validation::inCheckmate(Tile* king)
{   
    return false;
    bool check = inCheck(king);
    findValid(king);
    // then check if somebody can protect him.
    // FIX: make a full version of underAttack, that will take a color as a parameter,
    // and will return a set of all attacking tiles
    // then check for every tile in between king and the threatening pieces of an enemy
    // that they are not under attack of king's friend pieces, otherwise check
    // if they can move here with a usual canMoveTo check, and if can return false; 
    // and probably it's better to save somewhere the possible move to save the king;
    // Though no chess game show it as I know
    // And make a lambda underAttack, that will show method underAttack with certain 
    // arguments

}

bool Validation::inStalemate(bool color)
{
    // FIX: better to dispose of extra variables in future
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Tile* tile = m_board[x][y];
            char name = tile->m_piece_name;
            bool piece_color = tile->m_white_piece;
            if (name != 'e' && piece_color == color){
                findValid(tile);
                if (!empty()){
                    m_valid_moves.clear();
                    return false;
                }
            }
        }
    }
    return true;
}

bool Validation::runThrough(pint coord, pintr add, lambda stop_cond, lambda borders_cond)
{
    // here range is (from, to] or (from, to), depending on stop_cond
    coord.first += add.first, coord.second += add.second;
    while (borders_cond(coord)) {
        if (stop_cond(coord))
            return true;
        coord.first += add.first, coord.second += add.second;
    }
    return false;
}

bool Validation::onDiagonals(pintr coord, lambda stop_cond, lambda borders_cond)
{
    bool b1 = runThrough(coord, { 1,  1 }, stop_cond, borders_cond),
        b2 = runThrough(coord, { 1, -1 }, stop_cond, borders_cond),
        b3 = runThrough(coord, { -1,  1 }, stop_cond, borders_cond),
        b4 = runThrough(coord, { -1, -1 }, stop_cond, borders_cond);
    return b1 || b2 || b3 || b4;
}

bool Validation::fastDiagonals(pintr coord, lambda stop_cond, lambda borders_cond)
{
    return runThrough(coord, { 1,  1 }, stop_cond, borders_cond) ||
           runThrough(coord, { 1, -1 }, stop_cond, borders_cond) ||
           runThrough(coord, { -1,  1 }, stop_cond, borders_cond) ||
           runThrough(coord, { -1, -1 }, stop_cond, borders_cond);
}

bool Validation::onPerp(pintr coord, lambda stop_cond, lambda borders_cond)
{
    bool b1 = runThrough(coord, { 0,  1 }, stop_cond, borders_cond),
        b2 = runThrough(coord, { 0,  -1 }, stop_cond, borders_cond),
        b3 = runThrough(coord, { 1,  0 }, stop_cond, borders_cond),
        b4 = runThrough(coord, { -1, 0 }, stop_cond, borders_cond);
    return b1 || b2 || b3 || b4;
}

bool Validation::fastPerp(pintr coord, lambda stop_cond, lambda borders_cond)
{
    return runThrough(coord, { 0,  1 }, stop_cond, borders_cond) ||
           runThrough(coord, { 0,  -1 }, stop_cond, borders_cond) ||
           runThrough(coord, { 1,  0 }, stop_cond, borders_cond) ||
           runThrough(coord, { -1, 0 }, stop_cond, borders_cond);
}

void Validation::kingPotential(pintr coord, set<pint>& coords)
{
    int x = coord.first, y = coord.second;
    coords = { { x - 1, y + 1 }, { x, y + 1 }, { x + 1, y + 1 },
               { x - 1, y },                   { x + 1, y },
               { x - 1, y - 1 }, { x, y - 1 }, { x + 1, y - 1 } };
}

void Validation::knightPotential(pintr coord, set<pint>& coords)
{
    int x = coord.first, y = coord.second;
    coords = { { x - 2, y + 1 }, { x - 1, y + 2 }, { x + 1, y + 2 }, { x + 2, y + 1 },
               { x - 2, y - 1 }, { x - 1, y - 2 }, { x + 1, y - 2 }, { x + 2, y - 1 } };
}

bool Validation::underAttack(pintr coord)
{
    // FIX: better make it return threatening tile
    // and even better to return all threatening tile
    const bool turn = m_board.m_white_turn;

    // Aliases for shortness
    auto inBoard = [](pintr coord) -> bool {
        return coord.first >= 0 && coord.first < 8 && coord.second >= 0 && coord.second < 8;
        };
    auto occupied = [this](pintr coord) -> bool {
        return m_board[coord.first][coord.second]->m_piece_name != 'e';
        };
    auto differentColor = [this, turn, occupied](pintr coord) -> bool {
        // may be realized differently, wit either turn or color
        return m_board[coord.first][coord.second]->m_white_piece != turn && occupied(coord);
        };
    auto pieceName = [this](pintr coord) -> char {
        return m_board[coord.first][coord.second]->m_piece_name;
        };


    int x = coord.first, y = coord.second;
    set <pint> need_check;
    kingPotential(coord, need_check);
    for (auto move : need_check)
        if (inBoard(move) && pieceName(move) == 'K' && differentColor(move))
            return true;

    knightPotential(coord, need_check);
    for (auto move : need_check)
        if (inBoard(move) && pieceName(move) == 'N' && differentColor(move))
            return true;

    int k = turn ? 1 : -1;
    need_check = { {x - 1, y + k}, {x + 1, y + k} };
    for (auto move : need_check)
        if (inBoard(move) && pieceName(move) == 'P' && differentColor(move))
            return true;

    auto checkPerp = [&](pintr coord) {
        return differentColor(coord) && (pieceName(coord) == 'R' || pieceName(coord) == 'Q');
        };
    auto bordersAfter = [&](pintr coor) {
        return inBoard(coor) && (!occupied(coor) || differentColor(coor));
        };
    if (fastPerp(coord, checkPerp, bordersAfter))
        return true;

    auto checkDiagonals = [&](pintr coord) {
        return differentColor(coord) && (pieceName(coord) == 'B' || pieceName(coord) == 'Q');
        };
    return fastDiagonals(coord, checkDiagonals, bordersAfter);
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
    bool first_evaluation = true, may_exposure = false;
    pint add, prev_dir = {0, 0};
    set <pint> potenial_moves;


    // Aliases for shortness
    auto inBoard = [](pintr coord) -> bool {
        return coord.first >= 0 && coord.first < 8 && coord.second >= 0 && coord.second < 8;
        };
    auto occupied = [this](pintr coord) -> bool {
        return m_board[coord.first][coord.second]->m_piece_name != 'e';
        };
    auto differentColor = [this, turn, occupied](pintr coord) -> bool {
    // may be realized differently, wit either turn or color
        return m_board[coord.first][coord.second]->m_white_piece != turn && occupied(coord);
        };
    auto pieceName = [this](pintr coord) -> char {
        return m_board[coord.first][coord.second]->m_piece_name;
        };
    auto addValid = [this](pintr coord) -> void {
        m_valid_moves.emplace(m_board[coord.first][coord.second]);  // FIX: should be insert or emplace?
        // because m_board[x][y] does already exist, it doesn't have to be created
        };


    // Usefull formulas:
    auto findDirection = [](pintr beg, pintr end) -> pint {
        int X = end.first - beg.first;
        int Y = end.second - beg.second;
        pint add{ X > 0 ? 1 : -1,  Y > 0 ? 1 : -1 };
        if (!Y) add.second = 0;
        else if (!X) add.first = 0;
        return add;
        };
    auto notAlignKing = [&from, &king](pintr coord) {
        // checks that "coord" tile is not on the same line with "king" tile and "from" 
        // tile, with the least operations possible
        return (from.first - coord.first) * (coord.second - king.second) !=
            (from.second - coord.second) * (coord.first - king.first);
        };
    

    // For king:
    auto bordersAfter = [&](pintr coor) {
        return inBoard(coor) && (!occupied(coor) || differentColor(coor));
        };
    auto exposureKing = [&](pintr coord) -> bool {
        if (first_evaluation){
            X = from.first - king.first,
            Y = from.second - king.second;
            bool from_align_king = abs(X) == abs(Y) || !X || !Y;
            if (from_align_king){
                add = findDirection(king, from);
                auto bordersBetween = [king, from](pintr coord) -> bool {
                    return coord != king && coord != from;  // whitout "if(from_align_king)"
// checking this will lead to borders violation sometimess (only if "from" tile 
// is not aligned with "king" tile), be carefull
                    };
                bool nothing_between = !runThrough(king, add, occupied, bordersBetween);
                auto threatensToKing = [&](pintr coord) -> bool {
                    char name = pieceName(coord);
                    return differentColor(coord) && (name == 'R' || name == 'B' || name == 'Q');
                    //return "RBQ"sv.contains(pieceName(coord)); } // C++23
                    // this line means that on the same line with king and the piece
                    // there's some rook, bishop or queen.
                    // We shouldn't be afraid of a pawn or knight, or other king. 
                    };
                bool occurs_threat = runThrough(from, add, threatensToKing, bordersAfter);
                may_exposure = from_align_king && nothing_between && occurs_threat;
            }
            first_evaluation = false;
        }
        //pint dir = findDirection(from, coord);
        //bool another_dir = dir != prev_dir;
        //prev_dir = dir;
        return may_exposure /*&& another_dir */ && notAlignKing(coord);
        };
    auto canMoveKingTo = [&](pintr coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
            pieceName(coord) != 'K' && !underAttack(coord);
        };

    // For knight, rook, bishop and queen:
    auto canMoveTo = [&](pintr coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
            pieceName(coord) != 'K' && !exposureKing(coord);
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


    // For pawn:
    auto pawnCanEat = [&](pintr coord) -> bool {
        return inBoard(coord) && differentColor(coord) &&
            pieceName(coord) != 'K' && !exposureKing(coord);
        };
    auto pawnCanMove = [&](pintr coord) -> bool {
        return inBoard(coord) && !occupied(coord) && !exposureKing(coord);
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

    switch (piece)
    {
    case 'P':  // pawn
        addPawnMoves(from);
    break;
    case 'N':  // knight
        knightPotential(from, potenial_moves);
        for (auto coord : potenial_moves)
            if (canMoveTo(coord))
                addValid(coord);
    break;
    case 'K':  // king
        kingPotential(from, potenial_moves);
        for (auto coord : potenial_moves)
            if (canMoveKingTo(coord))
                addValid(coord);
    break;
    case 'B':  // bishop
        onDiagonals(from, addMove, inBoard);
    break;
    case 'R':  // rook
        onPerp(from, addMove, inBoard);
    break;
    case 'Q':  // queen
        onPerp(from, addMove, inBoard);
        onDiagonals(from, addMove, inBoard);
    };
}


// FIX: we also need to add castling, transformation, en passant,

// FIX: when king is under check add to valid_moves only that moves, that will protect 
// him.
// Possibly, we can add method willProtect(const set<pint>& threats, set<move> valid_moves)
// where move is pair<int8_t, int8_t> with two numbers of tiles (k, or n in future)


