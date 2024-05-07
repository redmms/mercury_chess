#pragma once
#include "../app/local_types.h"
#include "virtual_validator.h"
#include <string>
#include <vector>

namespace mmd
{
    class VirtualTile;
    class Tile;
    class Board;
    class VirtualBoard
    {
    protected:
        VirtualValidator* valid;
        VirtualTile* tiles[8][8];
        scoord from_coord;
        scoord black_king;
        scoord white_king;
        bool turn;
        bool side;
        int current_move;
        endnum end_type;
        bool end;
        std::vector<halfmove> history;
        int no_change_n;

    public:
        VirtualBoard();
        VirtualBoard(Board* copy_);
        virtual ~VirtualBoard() {}

        // getters:
        VirtualValidator* Valid();
        VirtualTile* (&Tiles())[8][8];
        scoord FromCoord();
        scoord bKing();
        scoord wKing();
        bool Turn();
        bool Side();
        int CurrentMove();
        endnum EndType();
        bool End();
        const std::vector<halfmove>& History();
        int NoChangeN();
        virtual VirtualTile* theTile(scoord coord);
        // setters:
        void setEndType(endnum end_type_);
        void setHistory(const std::vector<halfmove>& history_);
        void setWKing(scoord white_king_);
        void setBKing(scoord black_king_);
        // move forward:
        void saveMoveSimply(scoord from, scoord to, vove& move);
        void moveSimply(scoord from, scoord to, bool virtually = false);
        void castleKing(scoord king, scoord destination, scoord rook, bool virtually = false);
        void passPawn(scoord from, scoord to, bool virtually = false);
        virtual void promotePawn(scoord from, char& into, bool virtually = true);
        virtual void halfMove(scoord from, scoord to, char promo, halfmove* saved = nullptr, bool virtually = false, bool historically = true);
        // move backward (undo):
        void restoreTile(const VirtualTile& saved, bool virtually = false);
        void revertMoveSimply(vove move, bool virtually = false);
        void revertCastling(vove move, bool virtually = false);
        void revertPass(vove move, bool virtually = false);
        inline void revertPromotion(vove move, bool virtually = false);
        void revertHalfmove(halfmove saved, bool virtually = false, bool historically = true);
        // methods for watching history:
        void moveForward();
        void moveBack();
        // tile initialization:
        void initTiles();
        void setTiles();
        void setTiles(QString fen);
        QString getFen();
        // other:
        std::string toStr(bool staticly = false);
        auto operator [](int i) {
            return tiles[i];
        }
        void importTiles(Tile* const (&arr)[8][8]);
        //void copyState(Board* orig);
    };
}  // namespace mmd