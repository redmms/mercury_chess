#pragma once
#include "../app/local_types.h"
#include "virtual_validator.h"
#include <string>
#include <vector>

class VirtualTile;
class Board;
class VirtualBoard {
public:
    VirtualValidator* valid;
    VirtualTile* tiles[8][8];
    scoord from_coord;
    scoord black_king;
    scoord white_king;
    bool turn;
    bool side;
    int current_move;
    endnum end_type;
    std::vector<halfmove> history;

    VirtualBoard();
    virtual ~VirtualBoard() {};

    // member access:
    virtual VirtualTile* theTile(scoord coord);
    bool theTurn();
    scoord wKing();
    scoord bKing();
    const std::vector<halfmove>& story();
    // move forward:
    void saveMoveSimply(scoord from, scoord to, vove& move);
    void moveSimply(scoord from, scoord to, bool virtually = false);
    void castleKing(scoord king, scoord destination, scoord rook, bool virtually = false);
    void passPawn(scoord from, scoord to, bool virtually = false);
    virtual void promotePawn(scoord from, char& into, bool virtually = true);
    virtual void halfMove(scoord from, scoord to, char promo);
    void halfMove(scoord from, scoord to, char promo, halfmove& saved, bool virtually = false);
    // move backward (undo):
    void restoreTile(const VirtualTile& saved, bool virtually = false);
    void revertMoveSimply(vove move, bool virtually = false);
    void revertCastling(vove move, bool virtually = false);
    void revertPass(vove move, bool virtually = false);
    void revertPromotion(vove move, bool virtually = false);
    void revertHalfmove(halfmove saved, bool virtually = false);
    // methods for watching history:
    void moveForward();
    void moveBack();
    // tile initialization:
    virtual void initTiles();
    void setTiles();
    void setTiles(QString fen);
    // other:
    std::string toStr(bool stat = false);
    auto operator [](int i) {
        return tiles[i];
    }
};