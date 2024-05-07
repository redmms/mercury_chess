#include "virtual_board.h"
#include "virtual_tile.h"
#include "tile.h"
#include "board.h"
#include <QDebug>
#include <QStringList>
#include <cctype>
#include <iostream>
using namespace std;

namespace mmd
{
    VirtualBoard::VirtualBoard() :
        valid(new VirtualValidator(this)),
        from_coord{ -1, -1 },  // always actualized in Tile::setPiece()
        white_king{ 4, 0 },  // ditto
        black_king{ 4, 7 },  // ditto
        turn(true),  // true for white turn;
        side(settings[match_side_e].toBool()),
        current_move(0),
        end_type(endnum::interrupt),
        end(false),
        no_change_n(0)
    {
        initTiles();
        setTiles();
    }

    VirtualBoard::VirtualBoard(Board* copy_)
    {
        *this = *copy_;
        initTiles();
        importTiles(copy_->Tiles());
        valid = new VirtualValidator(this);
        valid->setCheck(copy_->Valid()->Check());
        for (int i = 0; i < 6; ++i) {
            valid->setHasMoved(i, copy_->Valid()->HasMoved()[i]);
        }
    }

    VirtualTile* VirtualBoard::theTile(scoord coord)
    {
        return VirtualBoard::tiles[coord.x][coord.y];
    }

    void VirtualBoard::setEndType(endnum end_type_)
    {
        end_type = end_type_;
    }

    void VirtualBoard::setHistory(const vector<halfmove>& history_)
    {
        history = history_;
    }

    void VirtualBoard::setWKing(scoord white_king_)
    {
        white_king = white_king_;
    }

    void VirtualBoard::setBKing(scoord black_king_)
    {
        black_king = black_king_;
    }

    bool VirtualBoard::Turn()
    {
        return turn;
    }

    bool VirtualBoard::Side()
    {
        return side;
    }

    int VirtualBoard::CurrentMove()
    {
        return current_move;
    }

    endnum VirtualBoard::EndType()
    {
        return end_type;
    }

    bool VirtualBoard::End()
    {
        return end;
    }

    scoord VirtualBoard::wKing()
    {
        return white_king;
    }

    VirtualValidator* VirtualBoard::Valid()
    {
        return valid;
    }

    VirtualTile* (&VirtualBoard::Tiles())[8][8]
    {
        return tiles;
    }

    scoord VirtualBoard::FromCoord()
    {
        return from_coord;
    }

    scoord VirtualBoard::bKing()
    {
        return black_king;
    }

    const vector<halfmove>& VirtualBoard::History()
    {
        return history;
    }

    int VirtualBoard::NoChangeN()
    {
        return no_change_n;
    }

    void VirtualBoard::saveMoveSimply(scoord from, scoord to, vove& move)
    {
        move = { *theTile(from), *theTile(to) };
    }

    void VirtualBoard::moveSimply(scoord from, scoord to, bool virtually)
    {
        auto from_tile = theTile(from);
        auto to_tile = theTile(to);
        to_tile->setPiece(from_tile->PieceName(), from_tile->PieceColor(), virtually);
        from_tile->setPiece('e', 0, virtually);
        valid->updateHasMoved(from, to);
    }

    void VirtualBoard::castleKing(scoord king, scoord destination, scoord rook, bool virtually)
    {
        moveSimply(king, destination, virtually);
        int k = destination.x - king.x > 0 ? -1 : 1;
        int x = destination.x + k;
        int y = destination.y;
        scoord rook_destination{ x, y };
        moveSimply(rook, rook_destination, virtually);
    }

    void VirtualBoard::passPawn(scoord from, scoord to, bool virtually)
    {
        auto opp_to_tile = theTile({ to.x, from.y });
        moveSimply(from, to, virtually);
        opp_to_tile->setPiece('e', 0, virtually);
    }

    void VirtualBoard::halfMove(scoord from, scoord to, char promo, halfmove* hmove, bool virtually, bool historically)
    {
        halfmove saved;
        saveMoveSimply(from, to, saved.move);
        saved.check = valid->Check();
        saved.turn = turn;
        saved.moved = arrToChar(valid->HasMoved());
        saved.no_change_n = no_change_n;

        if (theTile(to)->PieceName() == 'e' && theTile(from)->PieceName() != 'P') {
            ++no_change_n;
        }
        else {
            no_change_n = 0;
        }

        size_t last_move;
        if (historically) {
            last_move = history.size() - 1;
        }
        else {
            last_move = current_move - 1;
        }

        scoord rook;
        if (valid->canCastle(from, to, &rook)) {
            castleKing(from, to, rook, virtually);
            saved.castling = true;
        }
        else if (0 <= last_move && last_move < history.size() && valid->canPass(from, to, history[last_move].move)) {
            passPawn(from, to, virtually);
            saved.pass = true;
        }
        else {
            moveSimply(from, to, virtually);
        }

        if (valid->canPromote(to, to)) {
            promotePawn(to, promo, virtually);  // promo will be changed by user
            saved.promo = promo;
        }

        if (historically) {
            history.push_back(saved);
        }

        if (hmove) {
            *hmove = saved;
        }

        if (!virtually) {
            turn = !turn;
            if (valid->inCheck(turn)) {
                if (valid->inStalemate()) {  // check + stalemate == checkmate
                    end_type = turn == side ? opponent_wins : user_wins;
                    end = true;
                }
            }
            else if (valid->inStalemate()) {
                end_type = draw_by_stalemate;
                end = true;
            }
        }

        //if (!virtually) { // use this version for faster testing
        //    turn = !turn;
        //    valid->inCheck(turn);
        //}

        //cout << "\n";
        //cout << toStr(false);
        //cout << "\n";
    }

    void VirtualBoard::restoreTile(const VirtualTile& saved, bool virtually)
    {
        auto tile = theTile(saved.Coord());
        tile->setPiece(saved.PieceName(), saved.PieceColor(), virtually);
    }

    void VirtualBoard::revertMoveSimply(vove move, bool virtually)
    {
        restoreTile(move.first, virtually);
        restoreTile(move.second, virtually);
    }

    void VirtualBoard::revertCastling(vove move, bool virtually)
    {
        scoord king = move.first.Coord();
        scoord destination = move.second.Coord();
        int add = destination.x - king.x > 0 ? 1 : -1;
        scoord rook{ destination.x - add, destination.y };
        scoord rook_corner{ add > 0 ? 7 : 0, destination.y };
        revertMoveSimply(move, virtually);
        auto corner_tile = theTile(rook_corner);
        auto rook_tile = theTile(rook);
        corner_tile->setPiece('R', rook_tile->PieceColor(), virtually);
        rook_tile->setPiece('e', 0, virtually);
    }

    void VirtualBoard::revertPass(vove move, bool virtually)
    {
        revertMoveSimply(move, virtually);
        scoord from = move.first.Coord();
        scoord to = move.second.Coord();
        auto opp_to_tile = theTile({ to.x, from.y });
        opp_to_tile->setPiece('P', !move.first.PieceColor(), virtually);

    }

    void VirtualBoard::revertPromotion(vove move, bool virtually)
    {
        revertMoveSimply(move, virtually);
    }

    void VirtualBoard::revertHalfmove(halfmove hmove, bool virtually, bool historically)
    {
        if (hmove.castling) {
            revertCastling(hmove.move, virtually);
        }
        else if (hmove.pass) {
            revertPass(hmove.move, virtually);
        }
        else if (hmove.promo != 'e') {
            revertPromotion(hmove.move, virtually);
        }
        else {
            revertMoveSimply(hmove.move, virtually);
        }
        valid->setCheck(hmove.check);
        bool has_moved_[6];
        charToArr(hmove.moved, has_moved_);
        valid->setHasMoved(has_moved_);
        no_change_n = hmove.no_change_n;
        if (historically) {
            history.pop_back();
        }
        if (!virtually) {
            turn = !turn;
            end_type = endnum::interrupt;
            end = false;
        }
    }

    void VirtualBoard::setTiles()
    {
        // black pawns
        for (int x = 0; x < 8; ++x) {
            theTile({ x, 6 })->setPiece('P', 0);
        }
        // white pawns
        for (int x = 0; x < 8; ++x) {
            theTile({ x, 1 })->setPiece('P', 1);
        }
        // black pieces
        string pieces = "RNBQKBNR";
        for (int x = 0; x < 8; ++x) {
            theTile({ x, 7 })->setPiece(pieces[x], 0);
        }
        // white pieces
        for (int x = 0; x < 8; ++x) {
            theTile({ x, 0 })->setPiece(pieces[x], 1);
        }
    }

    void VirtualBoard::setTiles(QString fen)
    {
        QStringList parts = fen.simplified().split(' ', Qt::SkipEmptyParts);
        QString tiles_fen = parts[0];
        QStringList raws = tiles_fen.split('/');
        scoord coord = { 0, 7 };
        for (int i = 0; i < raws.size(); ++i) {
            for (int j = 0; j < raws[i].size(); ++j) {
                QChar qc = raws[i][j];
                bool is_digit = qc.isDigit();
                if (is_digit) {
                    int n = QString(qc).toInt();
                    for (int r = coord.x + n; coord.x < r; ++coord.x) {
                        theTile(coord)->setPiece('e', 0);
                    }
                    coord.x--;
                }
                else {
                    bool color = qc.isUpper();
                    char piece = qc.toUpper().toLatin1();
                    theTile(coord)->setPiece(piece, color);
                }
                ++coord.x;
            }
            coord.y--;
            coord.x = 0;
        }
        QString turn_fen = parts[1];
        turn = turn_fen == "w";
        QString castle_fen = parts[2];
        for (int i = 0; i < 6; ++i)
            valid->setHasMoved(i, true);
        if (castle_fen.contains('Q')) {
            valid->setHasMoved(0, false);
            valid->setHasMoved(1, false);
        }
        if (castle_fen.contains('K')) {
            valid->setHasMoved(1, false);
            valid->setHasMoved(2, false);
        }
        if (castle_fen.contains('q')) {
            valid->setHasMoved(3, false);
            valid->setHasMoved(4, false);
        }
        if (castle_fen.contains('k')) {
            valid->setHasMoved(4, false);
            valid->setHasMoved(5, false);
        }
        QString no_change_fen = parts.size() > 4 ? parts[4] : "0";
        no_change_n = no_change_fen.toInt();
        QString fullmove_fen = parts.size() > 5 ? parts[5] : "1";
        int halfmove_count = fullmove_fen.toInt() * 2;
        if (turn_fen == "w")
            halfmove_count -= 2;
        else
            halfmove_count -= 1;
        if (halfmove_count < 1) {
            showBox("Invalid FEN", "Check turn color and next move index validity.");
            return;
        }
        halfmove hmove;
        history = vector<halfmove>(halfmove_count, hmove);
        QString pass_fen = parts[3];
        if (pass_fen == "-") {
            history.back().pass = false;
        }
        else {
            scoord pass_coord = stringToCoord(pass_fen);
            int k = turn ? 1 : -1;
            scoord from = { pass_coord.x, pass_coord.y + k };
            scoord to = { pass_coord.x, pass_coord.y - k };
            history.back().move = { {from, 'P', !turn}, {to, 'e', 0} };
            history.back().turn = !turn;
        }
        valid->inCheck(turn);
        valid->searchingInStalemate();
    }

    QString VirtualBoard::getFen()
    {
        QString tiles_fen = "";
        scoord coord = { 0, 7 };
        int empty_count = 0;
        for (int y = 7; y >= 0; --y) {
            for (int x = 0; x <= 7; ++x) {
                char c = theTile({ x, y })->PieceName();
                if (c != 'e') {
                    if (empty_count) {
                        tiles_fen.push_back('0' + empty_count);
                        empty_count = 0;
                    }
                    bool color = theTile({ x, y })->PieceColor();
                    QChar qc = c;
                    if (!color) {
                        qc = qc.toLower();
                    }
                    tiles_fen.push_back(qc);
                }
                else {
                    ++empty_count;
                }
            }
            if (empty_count) {
                tiles_fen.push_back('0' + empty_count);
                empty_count = 0;
            }
            tiles_fen.push_back('/');
        }
        tiles_fen.chop(1);
        QString turn_fen = turn ? "w" : "b";
        QString castle_fen = "KQkq";
        if (valid->HasMoved()[0]) {
            castle_fen.remove('Q');
        }
        if (valid->HasMoved()[1]) {
            castle_fen.remove('Q');
            castle_fen.remove('K');
        }
        if (valid->HasMoved()[2]) {
            castle_fen.remove('K');
        }
        if (valid->HasMoved()[3]) {
            castle_fen.remove('q');
        }
        if (valid->HasMoved()[4]) {
            castle_fen.remove('q');
            castle_fen.remove('k');
        }
        if (valid->HasMoved()[5]) {
            castle_fen.remove('k');
        }
        QString pass_fen;
        if (!history.empty()) {
            vove last = history.back().move;
            scoord from = last.first.Coord();
            scoord to = last.second.Coord();
            bool could_be_pass = abs(to.y - from.y) == 2;
            if (could_be_pass) {
                int k = last.first.PieceColor() ? 1 : -1;
                scoord middle = { to.x, int(4 - 1.5 * k) };
                pass_fen = coordToString(middle);
            }
            else {
                pass_fen = "-";
            }
        }
        else {
            pass_fen = "-";
        }
        QString no_change_fen = QString::number(no_change_n);
        float halfmove_n = history.size();
        int i_next_fullmove = ceil((halfmove_n + 1) / 2);
        QString fullmove_fen = QString::number(i_next_fullmove);
        QString fen = tiles_fen +
            " " +
            turn_fen +
            " " +
            castle_fen +
            " " +
            pass_fen +
            " " +
            no_change_fen +
            " " +
            fullmove_fen;
        return fen;
    }

    void VirtualBoard::moveForward()
    {
        if (0 <= current_move && current_move < history.size()) {
            halfmove hmove = history[current_move];
            scoord from = hmove.move.first.Coord();
            scoord to = hmove.move.second.Coord();
            char promo = hmove.promo;
            halfMove(from, to, promo, nullptr, false, false);
            ++current_move;
        }
    }

    void VirtualBoard::moveBack()
    {
        if (0 < current_move) {
            current_move--;
            if (current_move < history.size()) {
                revertHalfmove(history[current_move], false, false);
            }
        }
    }

    string VirtualBoard::toStr(bool staticly)
    {
        string view;
        for (int y = 7; y >= 0; --y) {
            for (int x = 0; x < 8; ++x) {
                auto tile = staticly ? VirtualBoard::theTile({ x, y }) : theTile({ x, y });
                view.push_back(tile->PieceColor() ? tile->PieceName() : tolower(tile->PieceName()));
            }
            view.push_back('\n');
        }
        return view;
    }

    void VirtualBoard::importTiles(Tile* const (&arr)[8][8])
    {
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                tiles[x][y]->setPiece(arr[x][y]->PieceName(), arr[x][y]->PieceColor());
            }
        }
    }

    void VirtualBoard::initTiles()
    {
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                tiles[x][y] = new VirtualTile({ x, y }, 'e', false, this);
            }
        }
    }

    void VirtualBoard::promotePawn(scoord from, char& into, bool virtually)
    {
        auto pawn_tile = theTile(from);
        pawn_tile->setPiece(into, pawn_tile->PieceColor(), virtually);
        valid->updateHasMoved(from, from);
    }
}  // namespace mmd