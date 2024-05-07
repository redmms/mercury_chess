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
    protected:
        VirtualBoard* board;
        std::set<scoord> movable_pieces;
        bool check;
        scoord rooks_kings[6];
        scoord castling_destination[6];
        std::list<scoord> should_be_free[6];
        std::list<scoord> should_be_safe[6];
        std::list<scoord> perp_dir;
        std::list<scoord> diag_dir;
        std::set<scoord> valid_moves;
        bool has_moved[6];

        // checks made through diagonal and perpendicular:
        std::function<void(scoord, std::set<scoord>&)> addValid;
        std::function<bool(scoord, scoord, lambda, lambda)> runThrough;
        std::function<bool(scoord, lambda, lambda, const std::list<scoord>&)> runLines;
        std::function<void(scoord, scoord, checker, bool&)> fastThrough;
        std::function<bool(scoord, checker, const std::list<scoord>&)> fastLines;
        std::function<bool(scoord, lambda, lambda, bool&)> enemyFinder;
        std::function <bool(scoord, lambda, lambda, std::set<scoord>&)> moveFinder;
        // possible move patterns (doesn't mean available at the moment) and other:
        void kingPotential(scoord coord, std::list<scoord>& coords);
        void castlingPotential(std::list<scoord>& coords);
        void knightPotential(scoord coord, std::list<scoord>& coords);
        void pawnEatPotential(scoord coord, std::list<scoord>& coords);
        void pawnMovePotential(scoord coord, std::list<scoord>& coords);
        bool underAttack(scoord coord);
        void moveVirtually(scoord from, scoord to, char promo, halfmove& saved_move);
        void revertVirtualMove(halfmove saved_move);
        // TODO: bool fastValid(scoord from);

    public:
        VirtualValidator(VirtualBoard* mother_board = 0);
        virtual ~VirtualValidator() {}

        // getters:
        virtual VirtualTile* theTile(scoord coord);
        bool Turn();
        scoord wKing();
        scoord bKing();
        const std::vector<halfmove>& History();
        const std::set<scoord>& ValidMoves();
        bool const (&HasMoved())[6];
        const std::set<scoord>& MovablePieces();
        bool Check();
        // setteers:
        void setCheck(bool check_);
        void setHasMoved(int i, bool moved);
        void setHasMoved(const bool (&has_moved_)[6]);
        // tile checks:
        std::function<bool(scoord)> inBoard;
        std::function<bool(scoord)> occupied;
        std::function<bool(scoord)> differentColor;
        std::function<bool(scoord)> sameColor;
        std::function<char(scoord)> pieceName;
        std::function<bool(scoord)> freeToEat;
        std::function<bool(scoord)> freeToPlace;
        std::function<bool(scoord)> pieceColor;
        // board checks:
        bool isValid(scoord move);
        bool empty();
        bool inCheck(bool color);
        bool inCheckmate(bool color);
        virtual bool inStalemate();
        bool searchingInStalemate();
        bool canCastle(scoord from, scoord to, scoord* rook = nullptr);
        bool canPass(scoord from, scoord to);
        bool canPass(scoord from, scoord to, const vove& last_move);
        bool canPromote(scoord pawn, scoord destination);
        void updateHasMoved(scoord from, scoord to);
        // other:
        void findValid(scoord from);
        void findValid(scoord from, std::set<scoord>& container);
        // testing:
#ifdef MMDTEST
        unsigned countMovesTest(int depth = 5, int i = 0);
        unsigned countMoves(int depth = 5, int i = 0);
        inline void printMoveCount(scoord from, scoord to, char promo, unsigned mc);
        inline unsigned tryMove(int depth, int i, scoord from, scoord to, char promo);
        inline unsigned countParticular(int depth, int i, scoord from);
        void printHasMoved();
#endif  // MMDTEST
    };
}  // namespace mmd