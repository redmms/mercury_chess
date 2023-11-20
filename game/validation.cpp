#pragma once
#include "validation.h"
#include "board.h"
#include "tile.h"
#include "local_types.h"
using namespace std;
using lambda = function<bool(scoord)>;
using checker = function<bool(scoord, bool&)>;
using func = bool (*)(scoord);

Validation::Validation(Board* mother_board) :
    board(*mother_board),
    theTile([this](scoord coord) -> Tile* {
            return board[coord.x][coord.y];
        }),
    inBoard( [](scoord coord) -> bool {
            return coord.x >= 0 && coord.x < 8 && coord.y >= 0 && coord.y < 8;
        }),
    occupied( [&](scoord coord) -> bool {
            return theTile(coord)->piece_name != 'e';
        }),
    differentColor([&](scoord coord) -> bool {
        // may be realized differently, with either turn or piece color
            return occupied(coord) && theTile(coord)->piece_color != board.turn;
        }),
    pieceName([&](scoord coord) -> char {
            return theTile(coord)->piece_name;
        }),
    addValid([&](scoord coord) -> void {
            valid_moves.emplace(theTile(coord));
        }),
    fastThrough([&](scoord coord, scoord add, checker stop_cond, bool& result) -> void {
        // here range is (from, to] or (from, to), depending on stop_cond
            do{ coord.x += add.x, coord.y += add.y; }
            while (!stop_cond(coord, result));
        }),
    fastLine([&](scoord coord, checker stop_cond, const list<scoord>& adds) -> bool {
            bool result = false;
            for (auto dir : adds){
                fastThrough(coord, dir, stop_cond, result);
                if (result)
                    return true;
            }
            return false;
        }),
    enemyFinder([&](scoord coord, lambda comparison, lambda borders, bool& result) -> bool {
            if (!borders(coord)){ // borders checking should always go first
                result = false; // then it's the end of a cycle
                return true;
            }
            else if (differentColor(coord)){
                result = comparison(coord);
                return true; // then stop
            }
            else
                return false; // then continue
        })
    {}

void Validation::showValid(Tile * from)
{
    findValid(from);
    if (!valid_moves.empty()) {
        for (auto tile : valid_moves)
            tile->dyeValid();
        from->dyeSelected();
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

bool Validation::inCheck(bool color)
{
    Tile* king = color ? board.white_king : board.black_king;
    return (check = underAttack(king->coord));
}

bool Validation::inCheckmate(bool color)
{   
    Tile* king = color ? board.white_king : board.black_king;
    return (check = underAttack(king->coord)) && inStalemate(king->piece_color);
}

bool Validation::inStalemate(bool color)
{
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (occupied({x, y}) && !differentColor({x, y})){
                findValid(theTile({x, y}));
                if (!empty()){
                    valid_moves.clear();
                    return false;
                }
            }
        }
    }
    return true;
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

bool Validation::underAttack(scoord coord)
{
    // better make it return threatening tile
    // and even better to return all threatening tiles,
    // but only if we need to add arrows for several possible moves like in chess.com

    list <scoord> need_check;
    kingPotential(coord, need_check);
    for (auto move : need_check)
        if (inBoard(move) && differentColor(move) && pieceName(move) == 'K' )
            return true;

    knightPotential(coord, need_check);
    for (auto move : need_check)
        if (inBoard(move) && differentColor(move) && pieceName(move) == 'N')
            return true;

    int x = coord.x, y = coord.y;
    int k = board.turn ? 1 : -1;
    need_check = { {x - 1, y + k}, {x + 1, y + k} };
    for (auto move : need_check)
        if (inBoard(move) && differentColor(move) && pieceName(move) == 'P')
            return true;

    auto bordersPerp = [&](scoord coord) {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord));
        };
    auto comparPerp = [&](scoord coord) {
        return pieceName(coord) == 'R' || pieceName(coord) == 'Q';
        };
    auto checkPerp = [&](scoord coord, bool&result) -> bool{
        return enemyFinder(coord, comparPerp, bordersPerp, result);
        };
    if (fastLine(coord, checkPerp, perp_dir))
        return true;
    
    auto comparDiag = [&](scoord coord) {
        return pieceName(coord) == 'B' || pieceName(coord) == 'Q';
        };
    auto checkDiag = [&](scoord coord, bool& result) -> bool{
        return enemyFinder(coord, comparDiag, bordersPerp, result);
        };
    return fastLine(coord, checkDiag, diag_dir);
}

void Validation::findValid(Tile *from_tile)
{
    // NOTE: for the use of this code, we consider that board goes [x][y]
    // instead of [i][j] that is equal to [y][x];
    // and we also consider that white pieces are on the 0 line and black on the 
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


    // Direction cycles:
    auto runThrough = [&](scoord coord, scoord add, lambda stop_cond, lambda borders_cond)
        {
            // here range is (from, to] or (from, to), depending on stop_cond
            coord.x += add.x, coord.y += add.y;
            while (borders_cond(coord)) {
                if (stop_cond(coord))
                    return true;
                coord.x += add.x, coord.y += add.y;
            }
            return false;
        };
    auto onDiagonals = [&](scoord coord, lambda stop_cond, lambda borders_cond)
        {
            bool res = false;
            for (scoord dir : diag_dir)
                res |= runThrough(coord, dir, stop_cond, borders_cond);
            return res;
        };
    auto onPerp = [&](scoord coord, lambda stop_cond, lambda borders_cond)
        {
        bool res = false;
        for (scoord dir : perp_dir)
            res |= runThrough(coord, dir, stop_cond, borders_cond);
        return res;
        };

    // Usefull formulas:
    auto findDirection = [](scoord beg, scoord end) -> scoord {
        // if two tiles are on the same diagonal or perpendicular
        // it will return unit vector, which can be used as increments for cycles, 
        // otherwise it's undefined behavior
        int X = end.x - beg.x;
        int Y = end.y - beg.y;
        scoord add{ X > 0 ? 1 : -1,  Y > 0 ? 1 : -1 };
        if (!Y) add.y = 0;
        else if (!X) add.x = 0;
        return add;
        };    
    auto notAlignKing = [&](scoord coord, scoord king, scoord from) {
        // checks that "coord" tile is not on the same line with "king" tile and "from"
        // tile, with the least operations possible
        return (from.x - coord.x) * (coord.y - king.y) !=
            (from.y - coord.y) * (coord.x - king.x);
    };

    // Lambdas about king:
    auto exposureKing = [&](scoord coord) -> bool {
        // FIX: how will it work if the king is already under check?
        if (first_evaluation){
            X = from.x - king.x,
            Y = from.y - king.y;
            bool from_aligns_king = abs(X) == abs(Y) || !X || !Y;
            if (from_aligns_king){
                add = findDirection(king, from);
                auto bordersBetween = [king, from](scoord coord) -> bool {
                    return coord != king && coord != from;
            // whitout "if(from_align_king)"
            // checking this will lead to borders violation sometimess (only if "from" tile
            // is not aligned with "king" tile), e.g. for knight; be carefull
                    };
                bool nothing_between = !runThrough(king, add, occupied, bordersBetween);
                auto threatensToKing = [&](scoord coord) -> bool {
                    char name = pieceName(coord);
                    return differentColor(coord) && (name == 'R' || name == 'B' || name == 'Q');
                    // return "RBQ"sv.contains(pieceName(coord)); } // C++23
                    // this line means that on the same line with king and the piece
                    // there's some rook, bishop or queen.
                    // We shouldn't be afraid of a pawn or knight, or other king. 
                    };
                auto bordersAfter = [&](scoord coord) {
                    return inBoard(coord) && (!occupied(coord) || differentColor(coord));
                    };
                auto checkerAfter = [&](scoord coord, bool& result) -> bool{
                    return enemyFinder(coord, threatensToKing, bordersAfter, result);
                };
                bool occurs_threat;
                fastThrough(from, add, checkerAfter, occurs_threat);
                may_exposure = from_aligns_king && nothing_between && occurs_threat;
            }
            first_evaluation = false;
        }
        return may_exposure && notAlignKing(coord, king, from);
        };
    auto letKingDie = [&](scoord coord){
        if (check){
            board.moveVirtually(from_tile, theTile(coord));
            bool check_remains = underAttack(king);
            board.revertMove(board.virtual_move);
            return check_remains;
        }
        return false;
    };
    auto castlingPotential = [&](std::list<scoord>& coords){
        // adds coords of potential castling destination for king to the list
        if (color)
            coords = {{2, 0}, {6, 0}}; 
        else
            coords = {{2, 7}, {6, 7}};
    };

    // Checker conditions:
    // For knight, rook, bishop and queen
    auto canMoveTo = [&](scoord coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
            pieceName(coord) != 'K' && !exposureKing(coord);
        };
    // For knight
    auto canMoveKnightTo = [&](scoord coord){
        return canMoveTo(coord) && !letKingDie(coord);
       };
    // For king
    auto canMoveKingTo = [&](scoord coord) -> bool {
        return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
            pieceName(coord) != 'K' && !underAttack(coord);
        };
    // For pawn
    auto pawnCanEat = [&](scoord coord) -> bool {
        return inBoard(coord) && !exposureKing(coord) && !letKingDie(coord) &&
        (!occupied(coord) && canPass(from_tile, theTile(coord)) || differentColor(coord) &&
            pieceName(coord) != 'K');
        };
    auto pawnCanMove = [&](scoord coord) -> bool {
        return inBoard(coord) && !occupied(coord) && !exposureKing(coord);
        };

    // Adding coords to this->valid_moves;
    auto addMove = [&](scoord coord) -> bool {  // FIX: should be reread
        bool let_die;
        if (canMoveTo(coord)) {
            let_die = letKingDie(coord);
            if (!let_die)
                addValid(coord);
                // don't add to valid_moves, but continue the cycle,
                // threat may be somewhere next on the line
            if (occupied(coord))
                return true; // add move but stop, we can eat but cannot go next
		    return false; // continue
        }
        else
            return true; // break cycle
        };
    auto addPawnMoves = [&](scoord coord) -> void {
        int x = coord.x, y = coord.y;
        scoord move;
        int k = turn ? 1 : -1;
        if (move = { x, y + 1 * k }; pawnCanMove(move)) {
            if (!letKingDie(move))
                addValid(move);
            if (move = { x, y + 2 * k }; (turn ? from.y == 1 : from.y == 6) &&
                                        pawnCanMove(move) && !letKingDie(move))
                addValid(move);
        }
        if (move = { x - 1, y + 1 * k }; pawnCanEat(move) && !letKingDie(move))
            addValid(move);
        if (move = { x + 1, y + 1 * k }; pawnCanEat(move) && !letKingDie(move))
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
            if (canMoveKnightTo(coord))
                addValid(coord);
    break;
    case 'K':  // king
        kingPotential(from, potenial_moves);
        for (auto coord : potenial_moves)
            if (canMoveKingTo(coord))
                addValid(coord);
        castlingPotential(potenial_moves);
        Tile* rook_stub;
        for (auto coord : potenial_moves)
            if (canCastle(from_tile, theTile(coord), &rook_stub))
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

bool Validation::canCastle(Tile* from, Tile* to, Tile** rook)
{
    std::list<int> castling_side;
    if (board.turn && from->piece_name == 'K' && !has_moved[1])
        castling_side = { 0, 2 };
    else if (!board.turn && from->piece_name == 'K' && !has_moved[4])
        castling_side = { 3, 5 };
    for (int i : castling_side)
        if (to->coord == castling_destination[i] && !has_moved[i]) {
            for (auto coord : should_be_free[i])
                if (theTile(coord)->piece_name != 'e')
                    return false;
            for (auto coord : should_be_safe[i])
                if (underAttack(coord))
                    return false;
            *rook = theTile(rooks_kings[i]);
            return true;
        }
    return false;
}

bool Validation::canPass(Tile* from, Tile* to)
{
    virtu opp_from = board.last_move.first;
    virtu opp_to = board.last_move.second;
    return (opp_from.name == 'P' &&
        opp_from.color != from->piece_color &&
        abs(opp_to.tile->coord.y - opp_from.tile->coord.y) == 2 &&
        opp_from.tile->coord.x == to->coord.x &&
        from->coord.y == opp_to.tile->coord.y);
}

bool Validation::canPromote(Tile* pawn, Tile* destination)
{
    return pawn->piece_name == 'P' && destination->coord.y == (board.turn ? 7 : 0);
}

void Validation::reactOnMove(Tile* from, Tile* to)
{
    for (int i = 0; i < 6; i++)
        if (from->coord == rooks_kings[i] || to->coord == rooks_kings[i])
            has_moved[i] = true;

}
