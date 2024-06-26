#pragma once
#include "virtual_board.h"
#include "validator.h"
#include <QLabel>

namespace mmd
{
    class VirtualTile;
    class Tile;
    class MainWindow;
    class Board : public QLabel, public VirtualBoard
    {
        Q_OBJECT
        Validator* valid;
        Tile* tiles[8][8];
        QString board_css;
        QString promo_css;
        int tile_size;
        std::vector<bitmove> bistory;

        void initLetter(int x, int y, int width, int height, QString ch);
        void drawLetters();
        void drawNumbers();
        void initTiles();
        char openPromotion(scoord from);

    public:
        Board(MainWindow* parent_, int size_);
        ~Board();

        // getters:
        Validator* Valid();
        QString BoardCss();
        QString PromoCss();
        int TileSize();
        const std::vector<bitmove>& Bistory();
        Tile* (&Tiles())[8][8];
        // setters:
        void setBistory(const std::vector<bitmove>& bistory_);
        // other:
        void promotePawn(scoord from, char& into, bool virtually = false) override;
        void halfMove(scoord from, scoord to, char promo, halfmove* saved = nullptr, bool virtually = false, bool historically = true) override;
        void savingHalfMove(scoord from, scoord to, char promo);
        void emitCurrentStatus(const halfmove& saved);
        Tile* theTile(scoord coord) override;
        void saveBitmove(scoord from, scoord to, bitmove& bmove);

    signals:
        void moveMade(scoord from, scoord to, char promotion_type);
        void theEnd(endnum end_type);
        void newStatus(estatus status);
        void promotionEnd();

    public slots:
        void reactOnClick(Tile* tile);
        void setTilesSlot(QString fen);
    };
}  // namespace mmd