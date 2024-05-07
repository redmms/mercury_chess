#include "validator.h"
#include "board.h"
#include <QDebug>
using namespace std;

namespace mmd
{
    Validator::Validator(Board* mother_board_) :
        VirtualValidator(mother_board_),
        board(mother_board_)
    {}

    Tile* Validator::theTile(scoord coord)
    {
        return board->theTile(coord);
    }

    void Validator::showValid(scoord from)
    {
        findValid(from);
        if (!valid_moves.empty()) {
            for (auto move : valid_moves)
                theTile(move)->dyeValid();
            theTile(from)->dyeSelected();
        }
    }

    void Validator::hideValid()
    {
        QString game_regime = settings[game_regime_e].toString();
        if (game_regime != historical) {
            scoord from = board->FromCoord();
            if (from != scoord{ -1, -1 })
                theTile(from)->dyeNormal();
            else
                qDebug() << "You try to dyeNormal() from_coord that is nullptr";
            for (auto move : valid_moves)
                theTile(move)->dyeNormal();
        }
        valid_moves.clear();
    }

    bool Validator::inStalemate()
    {
        // checks the current turn
        return searchingInStalemate();
    }
}  // namespace mmd