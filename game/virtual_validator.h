#pragma once
#include "../app/local_types.h"
//#include "virtual_tile.h"
//#include "virtual_board.h"
#include <functional>
#include <set>
using lambda = std::function<bool(scoord)>;
using checker = std::function<bool(scoord, bool&)>;

class VirtualTile;
class VirtualBoard;
class VirtualValidator {
    friend class Archiver;
    friend class Board;
    friend class VirtualBoard;
protected:
    VirtualBoard* vboard;
    std::set<scoord> movable_pieces;
    std::set<scoord> valid_moves;
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
    void findValid(scoord from);

public:
    VirtualValidator(VirtualBoard* mother_board = 0);

    std::function<bool(scoord)> inBoard;
    std::function<bool(scoord)> occupied;
    std::function<bool(scoord)> differentColor;
    std::function<char(scoord)> pieceName;

    virtual VirtualTile& theTile(scoord);
    virtual bool theTurn();
    virtual VirtualTile& theWKing();
    virtual VirtualTile& theBKing();
    virtual void moveVirtually(scoord, scoord, vove&);
    virtual void revertVirtualMove(vove&);
    virtual const std::vector<halfmove>& theStory();

    bool isValid(scoord move);
    bool empty();
    bool inCheck(bool color);
    bool inCheckmate(bool color);
    bool inStalemate(bool color);
    bool canCastle(scoord from, scoord to, scoord& rook);
    bool canPass(scoord from, scoord to);
    bool canPromote(scoord pawn, scoord destination);
    void reactOnMove(scoord from, scoord to);
    void bringBack(scoord from, scoord to);
    //qint64 countMovesTest(int depth = 5, int i = 0);
};