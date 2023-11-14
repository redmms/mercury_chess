#pragma once
#include "validation.h"
#include "board.h"
#include "local_types.h"
using namespace std;
//using pint = std::pair<int, int>;
//using pintr = const pint&;
using lambda = function<bool(scoord)>;
using func = bool (*)(scoord);

void Validation::showValid(Tile * from)
{
    findValid(from);
    if (!valid_moves.empty()) {
        for (auto tile : valid_moves)
            tile->setStyleSheet("background: orange;");
        from->setStyleSheet("background: green;");
    }
}

void Validation::hideValid()
{
    board.from_tile->dyeNormal();
    for (auto tile : valid_moves)
        tile->dyeNormal();
    valid_moves.clear();
}

bool Validation::isValid(Tile* tile)
{
    return valid_moves.count(tile);
}

bool Validation::empty()
{
    return valid_moves.empty();
}

bool Validation::inCheck(Tile* king)
{
    return underAttack(king->coord);
}

bool Validation::inCheckmate(Tile* king)
{   
    return false;
   // bool check = inCheck(king); // don't want extra operations
    findValid(king);
    if (!empty())
        return false;
    else{
        //for
        // надо как-то заполнить новый сэт координат между королем и угрозой
        // но для этого надо знать где угроза, и точно ли она одна? Ничто не мешает 
        // быть нескольким угрозам одновременно, особенно если съели пешку или т.п. ИЛи мешает?
        // если съедена пешка это может добавить только одну угрозу, однако, вражеский нападающий может открыть
        // собой другую угрозу.
    }
        
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

    valid_moves.clear();
}

bool Validation::inStalemate(bool color)
{
    // FIX: better to dispose of extra variables in future
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Tile* tile = board[x][y];
            char name = tile->piece_name;
            bool piece_color = tile->piece_color;
            if (name != 'e' && piece_color == color){
                findValid(tile);
                if (!empty()){
                    valid_moves.clear();
                    return false;
                }
            }
        }
    }
    return true;
}

bool Validation::runThrough(scoord coord, scoord add, lambda stop_cond, lambda borders_cond)
{
    // here range is (from, to] or (from, to), depending on stop_cond
    coord.x += add.x, coord.y += add.y;
    while (borders_cond(coord)) {
        if (stop_cond(coord))
            return true;
        coord.x += add.x, coord.y += add.y;
    }
    return false;
}

bool Validation::onDiagonals(scoord coord, lambda stop_cond, lambda borders_cond)
{
    bool b1 = runThrough(coord, { 1,  1 }, stop_cond, borders_cond),
        b2 = runThrough(coord, { 1, -1 }, stop_cond, borders_cond),
        b3 = runThrough(coord, { -1,  1 }, stop_cond, borders_cond),
        b4 = runThrough(coord, { -1, -1 }, stop_cond, borders_cond);
    return b1 || b2 || b3 || b4;
}

bool Validation::fastDiagonals(scoord coord, lambda stop_cond, lambda borders_cond)
{
    return runThrough(coord, { 1,  1 }, stop_cond, borders_cond) ||
           runThrough(coord, { 1, -1 }, stop_cond, borders_cond) ||
           runThrough(coord, { -1,  1 }, stop_cond, borders_cond) ||
           runThrough(coord, { -1, -1 }, stop_cond, borders_cond);
}

bool Validation::onPerp(scoord coord, lambda stop_cond, lambda borders_cond)
{
    bool b1 = runThrough(coord, { 0,  1 }, stop_cond, borders_cond),
        b2 = runThrough(coord, { 0,  -1 }, stop_cond, borders_cond),
        b3 = runThrough(coord, { 1,  0 }, stop_cond, borders_cond),
        b4 = runThrough(coord, { -1, 0 }, stop_cond, borders_cond);
    return b1 || b2 || b3 || b4;
}

bool Validation::fastPerp(scoord coord, lambda stop_cond, lambda borders_cond)
{
    return runThrough(coord, { 0,  1 }, stop_cond, borders_cond) ||
           runThrough(coord, { 0,  -1 }, stop_cond, borders_cond) ||
           runThrough(coord, { 1,  0 }, stop_cond, borders_cond) ||
           runThrough(coord, { -1, 0 }, stop_cond, borders_cond);
}

void Validation::kingPotential(scoord coord, list<scoord>& coords)
{
    int x = coord.x, y = coord.y;
    coords = { { x - 1, y + 1 }, { x, y + 1 }, { x + 1, y + 1 },
               { x - 1, y },                   { x + 1, y },
               { x - 1, y - 1 }, { x, y - 1 }, { x + 1, y - 1 } };
}

void Validation::knightPotential(scoord coord, list<scoord>& coords)
{
    int x = coord.x, y = coord.y;
    coords = { { x - 2, y + 1 }, { x - 1, y + 2 }, { x + 1, y + 2 }, { x + 2, y + 1 },
               { x - 2, y - 1 }, { x - 1, y - 2 }, { x + 1, y - 2 }, { x + 2, y - 1 } };
}

bool Validation::notAlignKing(scoord coord, scoord king, scoord from) {
    // checks that "coord" tile is not on the same line with "king" tile and "from" 
    // tile, with the least operations possible
    return (from.x - coord.x) * (coord.y - king.y) !=
        (from.y - coord.y) * (coord.x - king.x);
};

bool Validation::underAttack(scoord coord)
{
    // FIX: better make it return threatening tile
    // and even better to return all threatening tile
    bool turn = board.turn;

    // Aliases for shortness
    auto inBoard = [](scoord coord) -> bool {
        return coord.x >= 0 && coord.x < 8 && coord.y >= 0 && coord.y < 8;
        };
    auto occupied = [this](scoord coord) -> bool {
        return board[coord.x][coord.y]->piece_name != 'e';
        };
    auto differentColor = [this, turn, occupied](scoord coord) -> bool {
        // may be realized differently, wit either turn or color
        return board[coord.x][coord.y]->piece_color != turn && occupied(coord);
        };
    auto pieceName = [this](scoord coord) -> char {
        return board[coord.x][coord.y]->piece_name;
        };


    int x = coord.x, y = coord.y;
    list <scoord> need_check;
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

    auto checkPerp = [&](scoord coord) {
        return differentColor(coord) && (pieceName(coord) == 'R' || pieceName(coord) == 'Q');
        };
    auto bordersAfter = [&](scoord coor) {
        return inBoard(coor) && (!occupied(coor) || differentColor(coor));
        };
    if (fastPerp(coord, checkPerp, bordersAfter))
        return true;

    auto checkDiagonals = [&](scoord coord) {
        return differentColor(coord) && (pieceName(coord) == 'B' || pieceName(coord) == 'Q');
        };
    return fastDiagonals(coord, checkDiagonals, bordersAfter);
}

void Validation::findValid(Tile *from_tile)
{
    // NOTE: for the purpose of this code, we consider that board goes [x][y]
    // instead of [i][j] that is equal to [y][x];
    // and we also consider that white piece are on the 0 line and black on the 
    // 7 line
    const bool turn = board.turn;
    const char piece = from_tile->piece_name;
    const bool color = from_tile->piece_color;
    const scoord king = color ? board.white_king->coord : board.black_king->coord;
    const scoord from = from_tile->coord;
    int X, Y;
    bool first_evaluation = true, may_exposure = false;
    scoord add;
    list <scoord> potenial_moves;

    // Aliases for shortness
    auto inBoard = [](scoord coord) -> bool {
        return coord.x >= 0 && coord.x < 8 && coord.y >= 0 && coord.y < 8;
        };
    auto occupied = [this](scoord coord) -> bool {
        return board[coord.x][coord.y]->piece_name != 'e';
        };
    auto differentColor = [this, turn, occupied](scoord coord) -> bool {
    // may be realized differently, wit either turn or color
        return board[coord.x][coord.y]->piece_color != turn && occupied(coord);
        };
    auto pieceName = [this](scoord coord) -> char {
        return board[coord.x][coord.y]->piece_name;
        };
    auto addValid = [this](scoord coord) -> void {
        valid_moves.emplace(board[coord.x][coord.y]);  // FIX: should be insert or emplace?
        // because board[x][y] does already exist, it doesn't have to be created
        };

    // Usefull formulas:
    auto findDirection = [](scoord beg, scoord end) -> scoord {
        int X = end.x - beg.x;
        int Y = end.y - beg.y;
        scoord add{ X > 0 ? 1 : -1,  Y > 0 ? 1 : -1 };
        if (!Y) add.y = 0;
        else if (!X) add.x = 0;
        return add;
        };    

    // For king:
    auto exposureKing = [&](scoord coord) -> bool {
        if (first_evaluation){
            X = from.x - king.x,
            Y = from.y - king.y;
            bool froalign_king = abs(X) == abs(Y) || !X || !Y;
            if (froalign_king){
                add = findDirection(king, from);
                auto bordersBetween = [king, from](scoord coord) -> bool {
                    return coord != king && coord != from/*(coord.x != king.x || coord.y != king.y) && (coord.x != from.x || coord.y != from.y)*/;  // whitout "if(froalign_king)"
// checking this will lead to borders violation sometimess (only if "from" tile 
// is not aligned with "king" tile), be carefull
                    };
                bool nothing_between = !runThrough(king, add, occupied, bordersBetween);
                auto threatensToKing = [&](scoord coord) -> bool {
                    char name = pieceName(coord);
                    return differentColor(coord) && (name == 'R' || name == 'B' || name == 'Q');
                    //return "RBQ"sv.contains(pieceName(coord)); } // C++23
                    // this line means that on the same line with king and the piece
                    // there's some rook, bishop or queen.
                    // We shouldn't be afraid of a pawn or knight, or other king. 
                    };
                auto bordersAfter = [&](scoord coor) {
                    return inBoard(coor) && (!occupied(coor) || differentColor(coor));
                    };
                bool occurs_threat = runThrough(from, add, threatensToKing, bordersAfter);
                may_exposure = froalign_king && nothing_between && occurs_threat;
            }
            first_evaluation = false;
        }
        //scoord dir = findDirection(from, coord);
        //bool another_dir = dir != prev_dir;
        //prev_dir = dir;
        return may_exposure /*&& another_dir */ && notAlignKing(coord, king, from);
        };
    auto canMoveKingTo = [&](scoord coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
            pieceName(coord) != 'K' && !underAttack(coord);
        };

    // For knight, rook, bishop and queen:
    auto canMoveTo = [&](scoord coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
            pieceName(coord) != 'K' && !exposureKing(coord);
        };
    auto addMove = [&](scoord coord) -> bool {
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
    auto pawnCanEat = [&](scoord coord) -> bool {
        return inBoard(coord) && differentColor(coord) &&
            pieceName(coord) != 'K' && !exposureKing(coord);
        };
    auto pawnCanMove = [&](scoord coord) -> bool {
        return inBoard(coord) && !occupied(coord) && !exposureKing(coord);
        };
    auto addPawnMoves = [&](scoord coord) -> void {
        int x = coord.x, y = coord.y;
        scoord move;
        int k = turn ? 1 : -1;
        if (move = { x, y + 1 * k }; pawnCanMove(move)) {
            addValid(move);
            if (move = { x, y + 2 * k }; (turn ? from.y == 1 : from.y == 6) && pawnCanMove(move))  // FIX: add en passant
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
// Possibly, we can add method willProtect(const set<scoord>& threats, set<move> valid_moves)
// where move is pair<int8_t, int8_t> with two numbers of tiles (k, or n in future)

//!notAlignKing;
//
//if coord !notAlignKing (thus it align king)
//
//than return true, it can be added to valid,
//or return false, it's not checkmate
// also check that king cannot move and thus escape the threat
