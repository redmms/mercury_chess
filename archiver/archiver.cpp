#pragma once
#include "archiver.h"
#include "../game/validator.h"
//#include "../game/virtual_tile.h"
#include "../game/board.h"
//#include <QDebug>
//#include <sstream>
#include <iostream>


import finestream;
using namespace std;

//stringstream sout;

Archiver::Archiver() {}

int Archiver::writeGame(endnum end_type, const std::vector<bitmove>& history, QString filename)
{
    try {
        fsm::ofinestream pofs(filename.toStdWString());
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    fsm::ofinestream ofs(filename.toStdWString());
    if (history.empty()) {
        cerr << "You are trying to write an empty game" << endl;
        return 2;
    }
    string opp_name = settings["opp_name"].toString().toStdString();
    uint8_t char_num = opp_name.size();
    bool user_side = settings["match_side"].toBool();
    uint8_t moves_num = history.size();
    bitremedy end(end_type, 4, false);
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

int Archiver::writeMove(bitmove move, fsm::ofinestream& ofs) {
    ofs << move.piece
        << move.move;
    //sout << " piece: " << move.piece.toStr() << endl
    //       << " move: " << move.move.toStr() << endl;
    if (move.promo != no_promotion) {
        ofs << bitset<2>(int(move.promo));
        //sout << " promo: " << bitset<2>(int(move.promo)) << endl;
    }
    return 0;
}

int Archiver::readGame(endnum& end_type, std::vector<bitmove>& bistory, std::vector<halfmove>& history, QString filename)
{
    try {
        fsm::ifinestream pifs(filename.toStdWString());
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    fsm::ifinestream ifs(filename.toStdWString());

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
    settings["opp_name"].setValue(QString(opp_name.c_str()));

    bool user_side;
    ifs >> user_side;
    settings["match_side"].setValue(user_side);

    uint8_t moves_num;
    ifs >> moves_num;

    bitremedy end{ 0, 4, false };
    ifs >> end;
    end_type = endnum(int(end));
    
    //sout << " side: " << user_side
    //     << " moves: " << bitset<8>(moves_num)
    //     << " end: " << end.toStr() << endl;

    VirtualBoard board;
    VirtualValidator* valid = board.valid;
    valid->searchingInStalemate(true);
    for (int i = 0; i < moves_num; i++) {
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
        valid->valid_moves.clear();
        //cout << "\n" <<  order << ". " << halfmoveToString(hmove).toStdString() << " \n";
    }
    //cout << sout.str();
    return 0;
}

inline int Archiver::readMove(bitmove& bmove, halfmove& hmove, fsm::ifinestream& ifs, VirtualBoard& board) 
{
    VirtualValidator* valid = board.valid;
    valid->searchingInStalemate(valid->theTurn());
    bmove.piece.BITSN = fsm::intMinBits(valid->movable_pieces.size() - 1);
    ifs >> bmove.piece;
    if (bmove.piece >= valid->movable_pieces.size()) {
        cerr << "WARNING: piece idx is bigger than movable pieces available" << endl;
        return 1;
    }
    scoord from = *next(valid->movable_pieces.begin(), int(bmove.piece));

    valid->findValid(from);
    bmove.move.BITSN = fsm::intMinBits(valid->valid_moves.size() - 1);
    ifs >> bmove.move;
    if (bmove.move >= valid->valid_moves.size()) {
        cerr << "WARNING: move idx is bigger than valid moves available" << endl;
        return 2;
    }
    scoord to = *next(valid->valid_moves.begin(), int(bmove.move));
    //sout << " piece: " << bmove.piece.toStr()
    //    << " move: " << bmove.move.toStr();

    if (valid->canPromote(from, to)) {
        bitset<2> promo;
        ifs >> promo;
        bmove.promo = promnum(promo.to_ulong());
        //sout << " promo: " << promo;
    }
    //sout << endl;
    board.halfMove(from, to, char_by_promo[bmove.promo], &hmove);
    return 0;
}

bitmove Archiver::toBitmove(halfmove hmove, Validator* valid)
{
    scoord from = hmove.move.first.coord;
    scoord to = hmove.move.second.coord;
    return { toPieceIdx(from, valid), toMoveIdx(to, valid), promo_by_char[hmove.promo] };
}

bitremedy Archiver::toPieceIdx(scoord from, Validator* valid) {
    unsigned char piece_idx = distance(valid->movable_pieces.begin(), valid->movable_pieces.find(from));
    return { piece_idx, fsm::intMinBits(valid->movable_pieces.size() - 1), false };
}

bitremedy Archiver::toMoveIdx(scoord to, Validator* valid) {
    unsigned char move_idx = distance(valid->valid_moves.begin(), valid->valid_moves.find(to));
    return { move_idx, fsm::intMinBits(valid->valid_moves.size() - 1), false };
}

halfmove Archiver::toHalfmove(bitmove bmove, VirtualValidator* valid) {
    scoord from = *next(valid->movable_pieces.begin(), int(bmove.piece));
    scoord to = *next(valid->valid_moves.begin(), int(bmove.move)); 
     //you need to control that these Validator movable_pieces and valid_moves will be actual and valid at the moment
    halfmove hmove;
    hmove.move = { *valid->theTile(from), *valid->theTile(to) };
    hmove.promo = char_by_promo[bmove.promo];
    hmove.castling = valid->canCastle(from, to);
    hmove.pass = valid->canPass(from, to);
    hmove.turn = valid->theTurn();
    return hmove;
}