#include "virtual_validator.h"
#include "virtual_board.h"
#include "virtual_tile.h"
#include "../app/mainwindow.h"
#include <iostream>
#ifdef MMDTEST
#include <iostream>
#include <string_view>
#endif
using namespace std;
using lambda = function<bool(mmd::scoord)>;
using checker = function<bool(mmd::scoord, bool&)>;

namespace mmd
{
    VirtualValidator::VirtualValidator(VirtualBoard* mother_board_) :
        board(mother_board_),
        valid_moves{},
        check{ false },
        has_moved{ false },
        rooks_kings{ {0,0}, {4, 0}, {7, 0}, {0, 7}, {4, 7}, {7, 7} },
        castling_destination{ {2, 0}, {-1, -1}, {6, 0}, {2, 7}, {-1, -1}, {6, 7} },
        should_be_free{
            {{3, 0}, {2, 0}, {1, 0}},
            {},
            {{5, 0}, {6, 0}},
            {{3, 7}, {2, 7}, {1, 7}},
            {},
            {{5, 7}, {6, 7}}
        },
        should_be_safe{
            {{4, 0}, {3, 0}, {2, 0}},
            {},
            {{4, 0}, {5, 0}, {6, 0}},
            {{4, 7}, {3, 7}, {2, 7}},
            {},
            {{4, 7}, {5, 7}, {6, 7}}
        },
        perp_dir{ { 0,  1 }, { 0,  -1 }, { 1,  0 },  { -1, 0 } },
        diag_dir{ { 1,  1 }, { 1, -1 },  { -1,  1 }, { -1, -1 } },
        inBoard([](scoord coord) -> bool {
            return 0 <= coord.x && coord.x < 8 && 0 <= coord.y && coord.y < 8;
            }),
        occupied([&](scoord coord) -> bool {
            return theTile(coord)->PieceName() != 'e';
            }),
        pieceName([&](scoord coord) -> char {
            return theTile(coord)->PieceName();
            }),
        pieceColor([&](scoord coord) -> bool {
            return theTile(coord)->PieceColor();
            }),
        differentColor([&](scoord coord) -> bool {
            // may be realized differently, with either turn or piece color
            return occupied(coord) && pieceColor(coord) != Turn();
            }),
        sameColor([&](scoord coord) -> bool {
            // may be realized differently, with either turn or piece color
            return occupied(coord) && pieceColor(coord) == Turn();
            }),
        freeToEat([&](scoord coord) -> bool {
            return differentColor(coord) && pieceName(coord) != 'K';
            }),
        freeToPlace([&](scoord coord) -> bool {
            return !occupied(coord) || freeToEat(coord);
            }),
        addValid([&](scoord coord, set<scoord>& container) -> void {
            container.emplace(coord);
            }),
        runThrough([&](scoord coord, scoord add, lambda stop_cond, lambda borders_cond) -> bool {
            // here range is (from, to] or (from, to), depending on stop_cond
            coord.x += add.x, coord.y += add.y;
            while (inBoard(coord) && borders_cond(coord)) {
                if (stop_cond(coord))
                    return true;
                coord.x += add.x, coord.y += add.y;
            }
            return false;
            }),
        runLines([&](scoord from, lambda stop_cond, lambda borders_cond, const list<scoord>& direction) -> bool {
            bool res = false;
            for (scoord add : direction)
                res |= runThrough(from, add, stop_cond, borders_cond);
            return res;
            }),
        fastThrough([&](scoord coord, scoord add, checker stop_cond, bool& result) -> void {
            // here range is (from, to] or (from, to), depending on stop_cond
            do { coord.x += add.x, coord.y += add.y; } while (inBoard(coord) && !stop_cond(coord, result));
                }),
            fastLines([&](scoord coord, checker stop_cond, const list<scoord>& direction) -> bool {
            bool result = false;
            for (auto add : direction) {
                fastThrough(coord, add, stop_cond, result);
                if (result)
                    return true;
            }
            return false;
            }),
        enemyFinder([&](scoord coord, lambda comparison, lambda borders, bool& result) -> bool {
            if (!borders(coord)) { // borders checking should always go first
                result = false;
                return true; // stop
            }
            else if (differentColor(coord)) {
                result = comparison(coord);
                return true; // stop
            }
            else
                return false; // continue
            }),
        moveFinder([&](scoord coord, lambda can_move_checker, lambda let_king_die_checker, set<scoord>& container) -> bool {
            if (can_move_checker(coord)) {
                if (!let_king_die_checker(coord))
                    addValid(coord, container);
                if (occupied(coord))
                    return true; // stop cycle
                else
                    return false; // continue
            }
            else {
                return true; // stop 
            }
            })
    {}

    VirtualTile* VirtualValidator::theTile(scoord coord)
    {
        return board->theTile(coord);
    }

    bool VirtualValidator::Turn()
    {
        return board->Turn();
    }

    scoord VirtualValidator::wKing()
    {
        return board->wKing();
    }

    scoord VirtualValidator::bKing()
    {
        return board->bKing();
    }

    const vector<halfmove>& VirtualValidator::History()
    {
        return board->History();
    }

    const std::set<scoord>& VirtualValidator::ValidMoves()
    {
        return valid_moves;
    }

    bool const (&VirtualValidator::HasMoved())[6]
    {
        return has_moved;
    }

    const std::set<scoord>& VirtualValidator::MovablePieces()
    {
        return movable_pieces;
    }

    bool VirtualValidator::Check()
    {
        return check;
    }

    void VirtualValidator::setCheck(bool check_)
    {
        check = check_;
    }

    void VirtualValidator::setHasMoved(int i, bool moved)
    {
        has_moved[i] = moved;
    }

    void VirtualValidator::setHasMoved(const bool(&has_moved_)[6])
    {
        for (int i = 0; i < 6; ++i) {
            has_moved[i] = has_moved_[i];
        }
    }

    void VirtualValidator::moveVirtually(scoord from, scoord to, char promo, halfmove& saved_move)
    {
        board->VirtualBoard::halfMove(from, to, promo, &saved_move, true, true);
    }

    void VirtualValidator::revertVirtualMove(halfmove saved_move)
    {
        board->VirtualBoard::revertHalfmove(saved_move, true, true);
    }

    void VirtualValidator::kingPotential(scoord coord, list<scoord>& coords)
    {
        int x = coord.x, y = coord.y;
        coords = { { x - 1, y + 1 }, { x, y + 1 }, { x + 1, y + 1 },
                   { x - 1, y },                   { x + 1, y },
                   { x - 1, y - 1 }, { x, y - 1 }, { x + 1, y - 1 } };
    }

    void VirtualValidator::castlingPotential(list<scoord>& coords)
    {
        if (Turn())
            coords = { {2, 0}, {6, 0} };
        else
            coords = { {2, 7}, {6, 7} };
    }

    void VirtualValidator::knightPotential(scoord coord, list<scoord>& coords)
    {
        int x = coord.x, y = coord.y;
        coords = { { x - 2, y + 1 }, { x - 1, y + 2 }, { x + 1, y + 2 }, { x + 2, y + 1 },
                   { x - 2, y - 1 }, { x - 1, y - 2 }, { x + 1, y - 2 }, { x + 2, y - 1 } };
    }

    void VirtualValidator::pawnEatPotential(scoord coord, list<scoord>& coords)
    {
        int x = coord.x, y = coord.y, k = Turn() ? 1 : -1;
        coords = { {x - 1, y + k}, {x + 1, y + k} };
    }

    void VirtualValidator::pawnMovePotential(scoord coord, list<scoord>& coords)
    {
        int x = coord.x, y = coord.y, k = Turn() ? 1 : -1;
        coords = { {x, y + k}, {x, y + 2 * k } };
    }

    bool VirtualValidator::underAttack(scoord coord)
    {
        // better make it return threatening tile
        // and even better to return all threatening tiles,
        // but only if we need to add arrows for several possible moves like in chess.com

        list <scoord> need_check;
        kingPotential(coord, need_check);
        for (auto move : need_check)
            if (inBoard(move) && differentColor(move) && pieceName(move) == 'K')
                return true;

        knightPotential(coord, need_check);
        for (auto move : need_check)
            if (inBoard(move) && differentColor(move) && pieceName(move) == 'N')
                return true;

        pawnEatPotential(coord, need_check);
        for (auto move : need_check)
            if (inBoard(move) && differentColor(move) && pieceName(move) == 'P')
                return true;

        auto comparePerp = [&](scoord coord) {
            return pieceName(coord) == 'R' || pieceName(coord) == 'Q';
            };
        auto checkPerp = [&](scoord coord, bool& result) -> bool {
            return enemyFinder(coord, comparePerp, freeToPlace, result);
            };
        if (fastLines(coord, checkPerp, perp_dir))
            return true;

        auto compareDiag = [&](scoord coord) {
            return pieceName(coord) == 'B' || pieceName(coord) == 'Q';
            };
        auto checkDiag = [&](scoord coord, bool& result) -> bool {
            return enemyFinder(coord, compareDiag, freeToPlace, result);
            };
        return fastLines(coord, checkDiag, diag_dir);
    }

    void VirtualValidator::findValid(scoord from)
    {
        valid_moves.clear();
        findValid(from, valid_moves);
    }

    void VirtualValidator::findValid(scoord from, set<scoord>& container)
    {
        // NOTE: for the use of this code, we consider that board goes [x][y]
        // instead of [i][j] that is equal to [y][x];
        // and we also consider that white pieces are on the 0 line and black on the 
        // 7 line
        //valid_moves.clear();
        auto from_tile = theTile(from);
        const bool turn = Turn();
        const char piece = from_tile->PieceName();
        const scoord king = turn ? wKing() : bKing();
        bool first_evaluation = true, may_expose = false;
        scoord add;
        list <scoord> potenial_moves;
        const int k = Turn() ? 1 : -1;

        // Usefull formulas:
        static auto findDirection = [](scoord beg, scoord end) {
            // if two tiles are on the same diagonal or perpendicular
            // it will return unit vector, which can be used as an increment for cycles, 
            // otherwise it's undefined behavior
            int X = end.x - beg.x;
            int Y = end.y - beg.y;
            scoord add{ X > 0 ? 1 : -1,  Y > 0 ? 1 : -1 };
            if (!Y) add.y = 0;
            if (!X) add.x = 0;
            return add;
            };
        auto notAlignsKing = [&](scoord coord, scoord king, scoord from) {
            // checks that "coord" tile is not on the same line with "king" tile and "from"
            // tile, with the least operations possible
            return (from.x - coord.x) * (coord.y - king.y) !=
                (from.y - coord.y) * (coord.x - king.x);
            };

        // Lambdas about king:
        auto exposureKing = [&](scoord coord) {
            if (first_evaluation) {
                int X = from.x - king.x,
                    Y = from.y - king.y;
                bool from_aligns_king = abs(X) == abs(Y) || !X || !Y;
                if (from_aligns_king) {
                    add = findDirection(king, from);
                    auto bordersBetween = [king, from](scoord coord) {
                        return coord != king && coord != from;
                        // whithout "if(from_align_king)"
                        // checking this will lead to borders violation sometimess (only if "from" tile
                        // is not aligned with "king" tile), e.g. for knight; be carefull
                        // also cycle may potentially leave the borders if from is equal king 
                        // (the moving piece is king)
                        };
                    bool nothing_between = !runThrough(king, add, occupied, bordersBetween);
                    auto threatensToKing = [&](scoord coord) {
                        char name = pieceName(coord);
                        return (abs(X) == abs(Y)
                            ? differentColor(coord) && (name == 'B' || name == 'Q')
                            : differentColor(coord) && (name == 'R' || name == 'Q'));
                        };
                    auto checkerAfter = [&](scoord coord, bool& result) {
                        return enemyFinder(coord, threatensToKing, freeToPlace, result);
                        };
                    bool occurs_threat = false;
                    fastThrough(from, add, checkerAfter, occurs_threat);
                    may_expose = from_aligns_king && nothing_between && occurs_threat;
                }
                first_evaluation = false;
            }
            return may_expose && notAlignsKing(coord, king, from); // if not aligns king then exposure him
            // otherwise it will protect king with its own body
            };
        auto letKingDie = [&](scoord coord) {
            // actually canPass() check should be in exposureKing() logically,
            // but it will cause an exact copy of this function in eposureKing()
            if (check || canPass(from, coord)) {
                halfmove virtual_move;
                moveVirtually(from, coord, 'Q', virtual_move);
                bool check_remains =
                    pieceName(coord) == 'K' ? underAttack(coord) : underAttack(king);
                revertVirtualMove(virtual_move);
                return check_remains;
            }
            return false;
            };

        // moveFinder() checker conditions for knight, rook, bishop and queen
        auto canMoveToSimply = [&](scoord coord) {
            return freeToPlace(coord) && !exposureKing(coord);
            };
        auto canMoveKnightTo = [&](scoord coord) {
            return freeToPlace(coord) && !exposureKing(coord) && !letKingDie(coord);
            };
        // For king
        auto canMoveKingTo = [&](scoord coord) {
            return freeToPlace(coord) && !underAttack(coord) && !letKingDie(coord);
            };
        // For pawn
        auto pawnCanEat = [&](scoord coord) {
            return (freeToEat(coord) || canPass(from, coord)) && !exposureKing(coord) && !letKingDie(coord);
            };
        auto pawnCanMove = [&](scoord coord) {
            bool hop = abs(coord.y - from.y) == 2;
            scoord middle = { coord.x, int(4 - 1.5 * k) };
            bool start_line = turn ? from.y == 1 : from.y == 6;
            return !occupied(coord) && !exposureKing(coord) && !letKingDie(coord) && (!hop || !occupied(middle) && start_line);
            };

        // Adding final coords to container:
        auto moveAdder = [&](scoord coord) {
            return moveFinder(coord, canMoveToSimply, letKingDie, container);
            };
        auto simpleMoveAdder = [&](lambda move_checker) {
            for (auto coord : potenial_moves)
                if (inBoard(coord) && move_checker(coord))
                    addValid(coord, container);
            };

        switch (piece)
        {
        case 'P':  // pawn
            pawnMovePotential(from, potenial_moves);
            simpleMoveAdder(pawnCanMove);
            pawnEatPotential(from, potenial_moves);
            simpleMoveAdder(pawnCanEat);
            break;
        case 'N':  // knight
            knightPotential(from, potenial_moves);
            simpleMoveAdder(canMoveKnightTo);
            break;
        case 'K':  // king
            kingPotential(from, potenial_moves);
            simpleMoveAdder(canMoveKingTo);
            castlingPotential(potenial_moves);
            simpleMoveAdder([&](scoord coord) {return canCastle(from, coord); });
            break;
        case 'B':  // bishop
            runLines(from, moveAdder, inBoard, diag_dir);
            break;
        case 'R':  // rook
            runLines(from, moveAdder, inBoard, perp_dir);
            break;
        case 'Q':  // queen
            runLines(from, moveAdder, inBoard, perp_dir);
            runLines(from, moveAdder, inBoard, diag_dir);
        }
    }

    bool VirtualValidator::isValid(scoord coord)
    {
        return valid_moves.count(coord);
    }

    bool VirtualValidator::empty()
    {
        return valid_moves.empty();
    }

    bool VirtualValidator::inCheck(bool color)
    {
        // updates 'check' variable
        auto king = color ? wKing() : bKing();
        return (check = underAttack(king));
    }

    bool VirtualValidator::inCheckmate(bool color)
    {
        auto king = color ? wKing() : bKing();
        cout << check << endl;
        return (check = underAttack(king)) && inStalemate();
    }

    bool VirtualValidator::inStalemate()
    {
        // checks the current turn
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                scoord coord{ x, y };
                if (occupied(coord) && !differentColor(coord)) {
                    set<scoord> temp_valid_moves;
                    findValid(coord, temp_valid_moves); // should fastFindValid without temp_moves parameter
                    if (!temp_valid_moves.empty()) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool VirtualValidator::searchingInStalemate()
    {
        // checks the current turn
        movable_pieces.clear();
        set<scoord> temp_valid_moves;
        bool stalemate = true;
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                scoord coord{ x, y };
                if (occupied(coord) && !differentColor(coord)) {
                    findValid(coord, temp_valid_moves); // should fastFindValid without temp_moves parameter
                    if (!temp_valid_moves.empty()) {
                        temp_valid_moves.clear();
                        movable_pieces.insert(coord);
                        stalemate = false;
                    }
                }
            }
        }
        return stalemate;
    }

    bool VirtualValidator::canCastle(scoord from, scoord to, scoord* rook)
    {
        list<int> castling_sides{};
        if (Turn() && pieceName(from) == 'K' && !has_moved[1])
            castling_sides = { 0, 2 };
        else if (!Turn() && pieceName(from) == 'K' && !has_moved[4])
            castling_sides = { 3, 5 };
        for (int i : castling_sides) {
            if (to == castling_destination[i] && !has_moved[i]) {
                for (auto coord : should_be_free[i])
                    if (occupied(coord))
                        return false;
                for (auto coord : should_be_safe[i])
                    if (underAttack(coord))
                        return false;
                if (rook)
                    *rook = rooks_kings[i];
                return true;
            }
        }
        return false;
    }

    bool VirtualValidator::canPass(scoord from, scoord to)
    {
        if (History().empty())
            return false;
        vove last_move = History().back().move;
        return canPass(from, to, last_move);
    }

    bool VirtualValidator::canPass(scoord from, scoord to, const vove& last_move)
    {
        VirtualTile opp_from_tile = last_move.first;
        VirtualTile opp_to_tile = last_move.second;
        scoord opp_from = last_move.first.Coord();
        scoord opp_to = last_move.second.Coord();
        return opp_from_tile.PieceName() == 'P' &&
            opp_from_tile.PieceColor() != pieceColor(from) &&
            abs(opp_to.y - opp_from.y) == 2 &&
            // it was pawn of opp color moved by 2 tiles
            pieceName(from) == 'P' &&
            // we are moving a pawn
            to.x == opp_from.x && // on the same column
            to.y == (opp_from.y + opp_to.y) / 2 && // on the raw between 2 enemy positions
            abs(from.x - opp_to.x) == 1 && // eating from the adjust column
            from.y == opp_to.y; // eating from the same raw
    }


    bool VirtualValidator::canPromote(scoord pawn, scoord destination)
    {
        return pieceName(pawn) == 'P' && destination.y == (Turn() ? 7 : 0);
    }

    void VirtualValidator::updateHasMoved(scoord from, scoord to)
    {
        for (int i = 0; i < 6; ++i) {
            if (from == rooks_kings[i] || to == rooks_kings[i]) {
                has_moved[i] = true;
            }
        }
    }

#ifdef MMDTEST
    void VirtualValidator::printHasMoved()
    {
        cout << '\n';
        for (int i = 0; i < 3; ++i) {
            cout << has_moved[i] << ' ';
        }
        cout << '\n';
        for (int i = 3; i < 6; ++i) {
            cout << has_moved[i] << ' ';
        }
        cout << '\n';
    }

    unsigned VirtualValidator::countMovesTest(int depth, int i)
    {
        //VirtualBoard board_copy = *board;
        unsigned total_moves_n = countMoves(depth, i);
        //for (int x = 0; x < 8; ++x)
        //    for (int y = 0; y < 8; ++y)
        //        if (board_copy[x][y] != (*board)[x][y])
        //            cout << "WARNING: Board is not the same after tests." << endl
        //                << "Board before:" << endl
        //                << board_copy.toStr() << endl
        //                << "Board after:" << endl
        //                << board->toStr() << endl << endl;
        return total_moves_n;
    }

    unsigned VirtualValidator::countMoves(int depth, int i)
    {
        unsigned total_count = 0;
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                scoord from = { x, y };
                if (sameColor(from))
                    total_count += countParticular(depth, i, from);
            }
        }
        return total_count;
    }

    unsigned VirtualValidator::countParticular(int depth, int i, scoord from)
        // last_from == {7, 1} && last_to == {7, 2} è i == 1 (2)
    {
        //auto size = board->history.size();
        //scoord last_from;
        //scoord last_to;
        //if (board->history.size() > 2) {
        //    last_from = board->history[2].move.first.coord;
        //    last_to = board->history[2].move.second.coord;
        //}
        //scoord next_from_last;
        //scoord next_to_last;
        //if (board->history.size() > 3) {
        //    next_from_last = board->history[3].move.first.coord;
        //    next_to_last = board->history[3].move.second.coord;
        //}
        //  auto debugPause = [&](scoord to) {
        //    return last_from == b4 &&
        //        last_to == c5 &&
        //        from == e8 &&
        //        to == d8;
        //    };
        //auto debugBottomPause = [&]() {
        //    return last_from == b4 &&
        //        last_to == c5 &&
        //        next_from_last == e8 &&
        //        next_to_last == d8;
        //    };


        unsigned particular_count = 0;
        set<scoord> saved_moves;
        //bool pause = debugBottomPause() && from == a7 || pieceName({ 0, 7 }) == 'e';
        findValid(from, saved_moves);
        if (i == depth - 1) {
            particular_count = saved_moves.size();
            for (scoord to : saved_moves) {
                bool can_promote = canPromote(from, to);
                particular_count += 3 * can_promote;
                if (!i /*|| debugBottomPause()*/) {
                    string str = can_promote ? "QRNB" : "e";
                    for (char promo : str) {
                        //cout << "    depth: " << depth;
                        printMoveCount(from, to, promo, 1);
                    }
                }
            }
        }
        else {
            for (scoord to : saved_moves) {
                string str = canPromote(from, to) ? "QRNB" : "e";
                for (char promo : str) {
                    //bool pause = from == h2 && to == h3;
                    auto copy_count = tryMove(depth, i + 1, from, to, promo);
                    particular_count += copy_count;
                    if (!i /*|| debugPause(to)*/)
                        printMoveCount(from, to, promo, copy_count);
                }
            }
        }
        saved_moves.clear();
        return particular_count;
    }

    unsigned VirtualValidator::tryMove(int depth, int i, scoord from, scoord to, char promo)
    {
        halfmove last_move;
        //VirtualBoard board_copy = *board;
        board->VirtualBoard::halfMove(from, to, promo, &last_move);
        unsigned move_count = countMoves(depth, i);
        board->VirtualBoard::revertHalfmove(last_move);
        //for (int x = 0; x < 8; ++x) {
        //    for (int y = 0; y < 8; ++y) {
        //        if (*board_copy[x][y] != *(*board)[x][y])
        //            cout << "WARNING: Board is not the same after tests." << endl
        //            << "Board before:" << endl
        //            << board_copy.toStr() << endl
        //            << "Board after:" << endl
        //            << board->toStr() << endl << endl;
        //    }
        //}
        //if (board_copy.black_king != board->black_king) {
        //    cout << "REVERT ERROR" << endl;

        //}
        //if (board_copy.white_king != board->white_king) {
        //    cout << "REVERT ERROR" << endl;

        //}
        //if (board_copy.turn != board->turn) {
        //    cout << "REVERT ERROR" << endl;

        //}
        //if (board_copy.side != board->side) {
        //    cout << "REVERT ERROR" << endl;

        //}
        //for (int i = 0; i < 6; ++i) {
        //    if (board_copy.valid->has_moved[i] != board->valid->has_moved[i])
        //        cout << "REVERT ERROR" << endl;

        //}
        //if (board_copy.history.size() != board->history.size()) {
        //    cout << "REVERT ERROR" << endl;
        //}

        return move_count;
    }

    void VirtualValidator::printMoveCount(scoord from, scoord to, char promo, unsigned move_count)
    {
        cout << coordToString(from).toStdString() + coordToString(to).toStdString();
        if (promo != 'e')
            cout << char(tolower(promo));
        cout << ": "
            << move_count
            << endl;
    }
#endif  // MMDTEST
}  // namespace mmd