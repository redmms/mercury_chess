#pragma once
#include "../app/local_types.h"
#include <functional>
#include <set>

namespace mmd
{
    using lambda = std::function<bool(scoord)>;
    using checker = std::function<bool(scoord, bool&)>;

    class VirtualTile;
    class VirtualBoard;
    class VirtualValidator
    {
        friend class Archiver;
        friend class Board;
        friend class VirtualBoard;
    protected:
        VirtualBoard* board;
        std::set<scoord> movable_pieces;
        std::set<scoord> valid_moves;
        bool check;
        scoord rooks_kings[6];
        scoord castling_destination[6];
        std::list<scoord> should_be_free[6];
        std::list<scoord> should_be_safe[6];
        std::list<scoord> perp_dir;
        std::list<scoord> diag_dir;

        std::function<void(scoord, std::set<scoord>&)> addValid;
        std::function<bool(scoord, scoord, lambda, lambda)> runThrough;
        std::function<bool(scoord, lambda, lambda, const std::list<scoord>&)> runLines;
        std::function<void(scoord, scoord, checker, bool&)> fastThrough;
        std::function<bool(scoord, checker, const std::list<scoord>&)> fastLines;
        std::function<bool(scoord, lambda, lambda, bool&)> enemyFinder;
        std::function <bool(scoord, lambda, lambda, std::set<scoord>&)> moveFinder;

        void kingPotential(scoord coord, std::list<scoord>& coords);
        void castlingPotential(std::list<scoord>& coords);
        void knightPotential(scoord coord, std::list<scoord>& coords);
        void pawnEatPotential(scoord coord, std::list<scoord>& coords);
        void pawnMovePotential(scoord coord, std::list<scoord>& coords);
        bool underAttack(scoord coord);
        //bool fastValid(scoord from);
        void findValid(scoord from);
        void findValid(scoord from, std::set<scoord>& container);
        void moveVirtually(scoord from, scoord to, char promo, halfmove& saved_move);
        void revertVirtualMove(halfmove saved_move);

    public:
        bool has_moved[6];

        VirtualValidator(VirtualBoard* mother_board = 0);

        std::function<bool(scoord)> inBoard;
        std::function<bool(scoord)> occupied;
        std::function<bool(scoord)> differentColor;
        std::function<bool(scoord)> sameColor;
        std::function<char(scoord)> pieceName;
        std::function<bool(scoord)> freeToEat;
        std::function<bool(scoord)> freeToPlace;
        std::function<bool(scoord)> pieceColor;

        virtual VirtualTile* theTile(scoord coord);
        bool theTurn();
        scoord wKing();
        scoord bKing();
        const std::vector<halfmove>& story();

        bool isValid(scoord move);
        bool empty();
        bool inCheck(bool color);
        bool inCheckmate(bool color);
        virtual bool inStalemate(bool color);
        bool searchingInStalemate(bool color);
        bool canCastle(scoord from, scoord to, scoord* rook = nullptr);
        bool canPass(scoord from, scoord to);
        bool canPass(scoord from, scoord to, const vove& last_move);
        bool canPromote(scoord pawn, scoord destination);
        void updateHasMoved(scoord from, scoord to);
        // testing:
        unsigned countMovesTest(int depth = 5, int i = 0);
        unsigned countMoves(int depth = 5, int i = 0);
        inline void printMoveCount(scoord from, scoord to, char promo, unsigned mc);
        inline unsigned tryMove(int depth, int i, scoord from, scoord to, char promo);
        inline unsigned countParticular(int depth, int i, scoord from);
        void printHasMoved();
    };
}