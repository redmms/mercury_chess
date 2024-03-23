#pragma once
#include "virtual_board.h"
#include "validator.h"
#include <QLabel>

class VirtualTile;
class Tile;
class MainWindow;
class Board : public QLabel, public VirtualBoard 
{
    Q_OBJECT

public:
    Validator* valid;
    Tile* tiles[8][8];
    Tile* menu[4];
    QString board_css;
    QString promo_css;
    int tile_size;
    std::vector<bitmove> bistory;

    Board(MainWindow* parent_, QLabel* background_);
    ~Board();

    void initLetter(int x, int y, int width, int height, QString ch);
    void drawLetters();
    void drawNumbers();
    char openPromotion(scoord from);
    void promotePawn(scoord from, char& into, bool virtually = false) override;
    void halfMove(scoord from, scoord to, char promo) override;
    void savingHalfMove(scoord from, scoord to, char promo);
    void emitCurrentStatus(const halfmove& saved);
    Tile* theTile(scoord coord) override;
    void initTiles() override;
    void saveBitmove(scoord from, scoord to, bitmove& bmove);

signals:
    void moveMade(scoord from, scoord to, char promotion_type);
    void theEnd(endnum end_type);
    void newStatus(tatus status);
    void promotionEnd();

public slots:
    void reactOnClick(Tile* tile);
};
