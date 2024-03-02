#pragma once
#include "../game/tile.h"
#include <functional>
#include <set>
using lambda = std::function<bool(scoord)>;
using checker = std::function<bool(scoord, bool&)>;

class Board;
class Validator {
friend class Archiver;
friend class Board;
protected:
    Board& board;
    std::set<Tile*, TileCmp> movable_pieces;
    std::set<Tile*, TileCmp> valid_moves;
    bool check;
    bool has_moved[6];
    scoord rooks_kings[6];
    scoord castling_destination[6];
    std::list<scoord> should_be_free[6];
    std::list<scoord> should_be_safe[6];
    std::list<scoord> perp_dir;
    std::list<scoord> diag_dir;

    std::function<void(scoord)> addValid;
    std::function<void(scoord, scoord, checker, bool&)> fastThrough;
    std::function<bool(scoord, checker, const std::list<scoord>&)> fastLine;
    std::function<bool(scoord, lambda, lambda, bool&)> enemyFinder;

    void kingPotential(scoord coord, std::list<scoord>& coords);
    void knightPotential(scoord coord, std::list<scoord>& coords);
    bool underAttack(scoord coord);
    void findValid(Tile* from);

public:
    Validator(Board* mother_board);

    std::function<Tile*(scoord)> theTile;
    std::function<bool(scoord)> inBoard;
    std::function<bool(scoord)> occupied;
    std::function<bool(scoord)> differentColor;
    std::function<char(scoord)> pieceName;

    void showValid(Tile* from);
    void hideValid();
    bool isValid(Tile* move);
    bool empty();
    bool inCheck(bool color);
    bool inCheckmate(bool color);
    bool inStalemate(bool color);
    bool canCastle(Tile* from, Tile* to, Tile*& rook);
    bool canPass(Tile* from, Tile* to);
    bool canPassVirtually(Tile* from, Tile* to, pove virtual_move);
    bool canPromote(Tile* pawn, Tile* destination);
    void reactOnMove(scoord from, scoord to);
    void bringBack(scoord from, scoord to);
    qint64 countMovesTest(int depth = 5, int i = 0);
};