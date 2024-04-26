#include "board.h"
#include "tile.h"
#include "../archiver/archiver.h"
#include "../app/mainwindow.h"
#include <QEventLoop>
#include <QLayout>
using namespace std;

namespace mmd
{
    Board::Board(MainWindow* parent_, int size_) :
        QLabel(parent_),
        tile_size(size_ / 9),
        valid(new Validator(this)),
        tiles{ {nullptr} },
        board_css(
            "mmd--Board{"
                "background-color: rgb(170, 170, 125);"
                "border: 1 solid black;"
                "border-radius: 14px;}"
            "mmd--Tile{"
                "border-radius: 7;}"
        ),
        promo_css(
            "mmd--Tile{"
                "background-color: white;}:"
            "hover{"
                "background-color: rgb(170,85,127);}"
        )
    {
        resize(size_, size_);
        setToolTip("Think thoroughly");
        drawLetters();
        drawNumbers();
        initTiles();
        VirtualBoard::valid = Board::valid;
        setTiles(); // order matters here
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                VirtualBoard::tiles[x][y] = Board::tiles[x][y];
            }
        }
    }

    Board::~Board()
    {
        delete valid;
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                delete tiles[x][y];
            }
        }
    }

    void Board::initLetter(int x, int y, int width, int height, QString ch)
    {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(ch);
        QFont font = letter->font();
        font.setPointSize(tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
    }

    void Board::drawLetters() {
        string letters = side ? "abcdefgh" : "hgfedcba";
        int width = tile_size, height = tile_size / 2,
            x = tile_size / 2, y1 = this->height() - height, y2 = 0;
        for (char ch : letters) {
            initLetter(x, y1, width, height, QString(ch));
            initLetter(x, y2, width, height, QString(ch));
            x += width;
        }
    }

    void Board::drawNumbers() {
        string digits = side ? "87654321" : "12345678";
        int width = tile_size / 2, height = tile_size,
            x1 = 0, x2 = this->width() - width, y = tile_size / 2;
        for (char ch : digits) {
            initLetter(x1, y, width, height, QString(ch));
            initLetter(x2, y, width, height, QString(ch));
            y += height;
        }
    }

    void Board::reactOnClick(Tile* tile) {
        scoord coord = tile->coord;
        QString game_regime = settings[game_regime_e].toString();
        if (game_regime == "history" || game_regime == "friend_online" && turn != side) {
            return;
        }
        else if (from_coord == scoord{ -1, -1 }) { // 1st click
            if (valid->sameColor(coord)) {
                from_coord = coord;
                valid->showValid(coord);
            }
            else {
                emit newStatus(estatus::invalid_move);
            }
        }
        else { // 2d click
            if (valid->sameColor(coord)) {
                valid->hideValid();
                from_coord = coord;
                valid->showValid(coord);
            }
            else if (valid->isValid(coord)) {
                savingHalfMove(from_coord, coord, 'e');
                valid->hideValid(); // order matters here, valid->valid_moves is used by saveBitmove()
                from_coord = { -1, -1 };
            }
            else {
                emit newStatus(estatus::invalid_move);
            }
        }
    }

    char Board::openPromotion(scoord from)
    {
        QEventLoop loop;
        string pieces = "QNRB";
        char promo;
        scoord menu_coord = from;
        for (int i = 0, k = turn ? -1 : 1; i < 4; ++i, menu_coord.y += k) {
            menu[i] = (new Tile(menu_coord, pieces[i], turn, side, this));
            menu[i]->setStyleSheet(promo_css);
            menu[i]->raise();
            menu[i]->show();
            QObject::connect(menu[i], &Tile::tileClicked, [&](Tile* into) {
                promo = into->piece_name;
                emit promotionEnd();
                });
            QObject::connect(this, &Board::promotionEnd, &loop, &QEventLoop::quit);
        }
        for (int x = 0; x < 8; ++x)
            for (int y = 0; y < 8; ++y)
                tiles[x][y]->setEnabled(false);
        loop.exec();
        for (int i = 0; i < 4; ++i) {
            delete menu[i];
        }
        for (int x = 0; x < 8; ++x)
            for (int y = 0; y < 8; ++y)
                tiles[x][y]->setEnabled(true);
        return promo;
    }

    void Board::initTiles()
    {
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                tiles[x][y] = (new Tile({ x, y }, 'e', false, side, this));
                // uses Board::tile_side bool side to coordinate itself on board, i.e. setGeometry()
                QObject::connect(tiles[x][y], &Tile::tileClicked, this, &Board::reactOnClick);
            }
        }
    }

    Tile* Board::theTile(scoord coord)
    {
        return Board::tiles[coord.x][coord.y];
    }

    void Board::promotePawn(scoord from, char& into, bool virtually)
    {
        QString game_regime = settings[game_regime_e].toString();
        if (game_regime == "friend_offline" || game_regime == "training" || game_regime == "friend_online" && turn == side) {
            into = openPromotion(from);
        }
        theTile(from)->setPiece(into, theTile(from)->piece_color);
    }

    void Board::halfMove(scoord from, scoord to, char promo, halfmove* saved, bool virtually, bool historically)
    {
        halfmove hmove;
        if (!saved)
            saved = &hmove;
        VirtualBoard::halfMove(from, to, promo, saved, virtually, historically);
        emitCurrentStatus(*saved);
    }

    void Board::savingHalfMove(scoord from, scoord to, char promo)
    {
        halfmove hmove;
        bitmove bmove;
        saveBitmove(from, to, bmove); // order matters here
        VirtualBoard::halfMove(from, to, promo, &hmove, false, true);
        bmove.promo = promo_by_char.at(hmove.promo); // known after openPromotion() only
        bistory.push_back(bmove);
        emitCurrentStatus(hmove);
    }

    void Board::saveBitmove(scoord from, scoord to, bitmove& bmove)
    {
        QString game_regime = settings[game_regime_e].toString();
        if (game_regime == "friend_online" && turn != side) {
            valid->findValid(from); // updates valid_moves for move index
        }
        bmove.move = Archiver::toMoveIdx(to, *valid);
        if (history.empty() /*|| game_regime == "training"*/) { // first move
            valid->inStalemate(true); // updates movable_pieces for piece index
        }
        bmove.piece = Archiver::toPieceIdx(from, *valid);
    }

    void Board::emitCurrentStatus(const halfmove& saved)
    {
        auto from_tile = saved.move.first;
        auto to_tile = saved.move.second;
        scoord from = from_tile.coord;
        scoord to = to_tile.coord;

        if (saved.turn == side)
            emit moveMade(from, to, saved.promo);

        if (end)
            emit theEnd(end_type);
        else if (valid->check)
            emit newStatus(turn == side ? check_to_user : check_to_opponent);
        else if (saved.castling)
            emit newStatus(castling);
        else if (saved.promo != 'e')
            emit newStatus(promotion);
        else if (saved.pass ||
            from_tile.piece_name != 'e' &&
            to_tile.piece_name != 'e' &&
            from_tile.piece_color != to_tile.piece_color)
            emit newStatus(saved.turn == side ? opponent_piece_eaten : user_piece_eaten);
        else
            emit newStatus(just_new_turn);
    }

    void Board::setTilesSlot(QString fen)
    {
        setTiles(fen);
    }
}