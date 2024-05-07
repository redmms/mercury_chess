#include "archiver.h"
#include "../game/validator.h"
//#include "../game/virtual_tile.h"
#include "../game/board.h"
//#include <QDebug>
//#include <sstream>
#include <QVariant>
#include <iostream>
import finestream;
using namespace std;

//stringstream sout;

namespace mmd
{
    Archiver::Archiver() {}

    int Archiver::writeGame(endnum end_type, const vector<bitmove>& history, QString filename)
    {
        try {
            fn::ofinestream pofs(filename.toStdWString());
        }
        catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
        fn::ofinestream ofs(filename.toStdWString());
        if (history.empty()) {
            cerr << "You are trying to write an empty game" << endl;
            return 2;
        }
        string opp_name = settings[opp_name_e].toString().toStdString();
        if (opp_name.size() > 255) {
            cerr << "WARNING: in Archiver: opp_name size is bigger than 255" << endl;
        }
        uint8_t char_num = static_cast<uint8_t>(opp_name.size());
        bool user_side = settings[match_side_e].toBool();
        if (history.size() > 255) {
            cerr << "ERROR: you are trying to save a game longer than 255 halfmoves. This archiver version supports only 122 move games" << endl;
            return 3;
        }
        uint8_t moves_num = static_cast<uint8_t>(history.size());
        fn::bitremedy end{ int(end_type), 4, false };
        ofs << archiver_version
            << char_num
            << opp_name
            << user_side
            << moves_num
            << end;
        //sout << " side: " << user_side
        //     << " moves: " << bitset<8>(moves_num)
        //     << " end: " << end.toStr() << endl;
        for (auto bmove : history) {
            writeMove(bmove, ofs);
        }
        //cout << sout.str();
        //cout << "Ending bits number in the end of file is " << ofs.ExtraZerosN() << endl;
        return 0;
    }

    int Archiver::writeMove(bitmove bmove, fn::ofinestream& ofs) {
        ofs << bmove.piece
            << bmove.move;
        //sout << " piece: " << move.piece.toStr() << endl
        //       << " move: " << move.move.toStr() << endl;
        if (bmove.promo != no_promotion) {
            ofs << bitset<2>(int(bmove.promo));
            //sout << " promo: " << bitset<2>(int(bmove.promo)) << endl;
        }
        return 0;
    }

    int Archiver::readGame(endnum& end_type, vector<bitmove>& bistory, vector<halfmove>& history, QString filename)
    {
        try {
            fn::ifinestream pifs(filename.toStdWString());
        }
        catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
        fn::ifinestream ifs(filename.toStdWString());

        uint8_t version;
        ifs >> version;
        if (version != archiver_version) {
            cerr << "You are using an inappropriate archiver version for this file" << endl;
            return 2;
        }

        uint8_t char_num;
        ifs >> char_num;

        string opp_name(char_num, ' ');
        ifs >> opp_name;
        settings[opp_name_e].setValue(QString(opp_name.c_str()));

        bool user_side;
        ifs >> user_side;
        settings[match_side_e].setValue(user_side);

        uint8_t moves_num;
        ifs >> moves_num;

        fn::bitremedy end{ 0, 4, false };
        ifs >> end;
        end_type = endnum(int(end));

        //sout << " side: " << user_side
        //     << " moves: " << bitset<8>(moves_num)
        //     << " end: " << end.toStr() << endl;

        VirtualBoard board;
        VirtualValidator& valid = *board.Valid();
        valid.searchingInStalemate();
        for (int i = 0; i < moves_num; ++i) {
            //int order = i + 1;
            //if ( order % 2)
            //    sout << (order - 1) / 2 + 1 << "." << endl;
            bitmove bmove;
            halfmove hmove;
            int err = readMove(bmove, hmove, ifs, board);
            if (err) {
                //cout << sout.str();
                return err + 2;
            }
            bistory.push_back(bmove);
            history.push_back(hmove);
            //valid.valid_moves.clear();
            //cout << "\n" <<  order << ". " << halfmoveToString(hmove).toStdString() << " \n";
        }
        //cout << sout.str();
        return 0;
    }

    inline int Archiver::readMove(bitmove& bmove, halfmove& hmove, fn::ifinestream& ifs, VirtualBoard& board)
    {
        VirtualValidator& valid = *board.Valid();
        valid.searchingInStalemate();
        bmove.piece = { bmove.piece.Ucbyte(), fn::IntNonLeadingN(valid.MovablePieces().size() - 1), bmove.piece.MovedLeft()};
        ifs >> bmove.piece;
        if (bmove.piece >= valid.MovablePieces().size()) {
            cerr << "WARNING: piece idx is bigger than movable pieces available" << endl;
            return 1;
        }
        scoord from = *next(valid.MovablePieces().begin(), int(bmove.piece));

        valid.findValid(from);
        bmove.move = { bmove.move.Ucbyte(), fn::IntNonLeadingN(valid.ValidMoves().size() - 1), bmove.move.MovedLeft()};
        ifs >> bmove.move;
        if (bmove.move >= valid.ValidMoves().size()) {
            cerr << "WARNING: move idx is bigger than valid moves available" << endl;
            return 2;
        }
        scoord to = *next(valid.ValidMoves().begin(), int(bmove.move));
        //sout << " piece: " << bmove.piece.toStr()
        //    << " move: " << bmove.move.toStr();

        if (valid.canPromote(from, to)) {
            bitset<2> promo;
            ifs >> promo;
            bmove.promo = promnum(promo.to_ulong());
            //sout << " promo: " << promo;
        }
        //sout << endl;
        board.halfMove(from, to, char_by_promo.at(bmove.promo), &hmove);
        return 0;
    }

    bitmove Archiver::toBitmove(halfmove hmove, Validator& valid)
    {
        scoord from = hmove.move.first.Coord();
        scoord to = hmove.move.second.Coord();
        return { toPieceIdx(from, valid), toMoveIdx(to, valid), promo_by_char.at(hmove.promo) };
    }

    fn::bitremedy Archiver::toPieceIdx(scoord from, Validator& valid) {
        unsigned char piece_idx = distance(valid.MovablePieces().begin(), valid.MovablePieces().find(from));
        return { piece_idx, fn::IntNonLeadingN(valid.MovablePieces().size() - 1), false};
    }

    fn::bitremedy Archiver::toMoveIdx(scoord to, Validator& valid) {
        unsigned char move_idx = distance(valid.ValidMoves().begin(), valid.ValidMoves().find(to));
        return { move_idx, fn::IntNonLeadingN(valid.ValidMoves().size() - 1), false};
    }

    halfmove Archiver::toHalfmove(bitmove bmove, VirtualValidator& valid) {
        scoord from = *next(valid.MovablePieces().begin(), int(bmove.piece));
        scoord to = *next(valid.ValidMoves().begin(), int(bmove.move));
        //you need to control that these Validator movable_pieces and valid_moves will be actual and valid at the moment
        halfmove hmove;
        hmove.move = { *valid.theTile(from), *valid.theTile(to) };
        hmove.promo = char_by_promo.at(bmove.promo);
        hmove.castling = valid.canCastle(from, to);
        hmove.pass = valid.canPass(from, to);
        hmove.turn = valid.Turn();
        return hmove;
    }
}  // namespace mmd