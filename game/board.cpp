#pragma once
#include "board.h"
#include <QGridLayout>
#include "local_types.h"
#include <string>
#include <QEventLoop>
#include <QTimer>

Board::Board(QLabel* background = 0) :
    tile_size(background->width() / 9)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(396, 396);
    setMaximumSize(396, 396);
    setStyleSheet("Board{"
                       "background: rgb(170, 170, 120);" 
                       "border-radius: 14;"
                  "}"
                  "Tile{"
                        "border-radius: 7;"
                   "}"
                   );
    setToolTip("Think thoroughly");

    background->parentWidget()->layout()->replaceWidget(background, this); // replace ui board, by this class
    background->~QLabel();

    drawLetters();
    drawNumbers();
    drawTiles();
};

void Board::reactOnClick(Tile* tile) {
    if (from_tile == nullptr) { // if it's first click then pick the piece and 
    // show valid moves
        if (turn != tile->piece_color || tile->piece_name == 'e')
            return;
        // turn for black, but white try to move, or vice versa, or tile is empty
        else {
            from_tile = tile;
            valid.showValid(tile);
        }
    }
    else if (tile != from_tile && turn == tile->piece_color && tile->piece_name != 'e') {
    // if the second click
    // is on the piece of same color then pick it instead
        valid.hideValid();
        from_tile = tile;
        valid.showValid(tile);
    }
    else if (valid.isValid(tile)) { // if it's the second click and move is valid
    //then move pieces
        if (Tile* rook; valid.canCastle(from_tile, tile, &rook))
            castleKing(from_tile, tile, rook);
        else if (valid.canPass(from_tile, tile))
            passPawn(from_tile, tile);
        else
            moveNormally(from_tile, tile);

        if(valid.canPromote(tile, tile))
            openPromotion(tile);  // waits until the signal from a tile received

        from_tile = nullptr;
        turn = !turn;

        if (valid.inCheck(turn))
            if (valid.inStalemate(turn))  // check + stalemate == checkmate
                emit theEnd(turn ? endnum::black_wins : endnum::white_wins);
            else
                emit newStatus(setatus::check); 
        else if(valid.inStalemate(turn))
            emit theEnd(endnum::stalemate);
        else
            emit newStatus(setatus::new_turn);

        valid.reactOnMove(from_tile, tile);
    }
    else
        emit newStatus(setatus::invalid_move);
}


void Board::drawLetters() {
    int width = tile_size, height = tile_size/2,
        x = tile_size / 2, y = this->height() - height;
    for (char ch : "abcdefgh") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        x += width;
    }
    x = width / 2, y = 0;
    for (char ch : "abcdefgh") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        x += width;
    }
}

void Board::drawNumbers() {
    int width = tile_size / 2, height = tile_size,
        x = 0, y = tile_size / 2;
    for (char ch : "87654321") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        y += height;
    }
    x = this->width() - width, y = height / 2;
    for (char ch : "87654321") {
        QLabel* letter = new QLabel(this);
        letter->setGeometry(x, y, width, height);
        letter->setText(QString(ch));
        QFont font = letter->font();
        font.setPointSize(tile_size / 4);
        letter->setFont(font);
        letter->setAlignment(Qt::AlignCenter);
        y += height;
    }
}

void Board::drawTiles()
{
    int indent = tile_size / 2;
    int hor = indent;
    int ver = indent;
    for (int y = 7; y >= 0; y--){
        for (int x = 0; x < 8; x++){
            tiles[x][y] = new Tile(this);
            tiles[x][y]->tile_color = !((x + y) % 2);
            // is this field really needed?
// Yes, it is otherwise you can't connect the tile pointer to it's 
// index, when tileClicked signal is emitted
// and also we can't use gridlayout because its index goes
// upside down
            tiles[x][y]->coord = {x, y}; 
            tiles[x][y]->setGeometry(hor, ver, tile_size, tile_size);
            tiles[x][y]->dyeNormal();
            QObject::connect(tiles[x][y], &Tile::tileClicked, this, &Board::reactOnClick);
            hor += tile_size;
        }
        hor = indent;
        ver += tile_size;
    }

    //black pawns
    for (int x = 0; x < 8; x++)
        tiles[x][6]->setPiece('P', 0);

    //white pawns
    for (int x = 0; x < 8; x++)
        tiles[x][1]->setPiece('P', 1);

    tiles[0][7]->setPiece('R', 0);
    tiles[1][7]->setPiece('N', 0);
    tiles[2][7]->setPiece('B', 0);
    tiles[3][7]->setPiece('Q', 0);
    tiles[4][7]->setPiece('K', 0);
    tiles[5][7]->setPiece('B', 0);
    tiles[6][7]->setPiece('N', 0);
    tiles[7][7]->setPiece('R', 0);

    tiles[0][0]->setPiece('R', 1);
    tiles[1][0]->setPiece('N', 1);
    tiles[2][0]->setPiece('B', 1);
    tiles[3][0]->setPiece('Q', 1);
    tiles[4][0]->setPiece('K', 1);
    tiles[5][0]->setPiece('B', 1);
    tiles[6][0]->setPiece('N', 1);
    tiles[7][0]->setPiece('R', 1);
}

void Board::openPromotion(Tile* from)
{
    QEventLoop loop;
    QString sheet = "QLabel{background: white;}:hover{background-color: rgb(170,85,127);}";
    QRect geo = from->geometry();
    std::string pieces = "QRBN";
    int k = turn ? tile_size : -tile_size;
    for (int i = 0; i < 4; i++){
        menu[i] = new Tile(this);
        menu[i]->setGeometry(geo);
        menu[i]->move(geo.x(), geo.y() + k * i);
        menu[i]->setStyleSheet(sheet);
        menu[i]->setPiece(pieces[i], turn);
        menu[i]->raise();
        menu[i]->show();
        connect(menu[i], &Tile::tileClicked, this, &Board::promotePawn);
        connect(this, &Board::promotionEnd, &loop, &QEventLoop::quit);
    }
    for (int x = 0; x < 8; x++){
        for (int y = 0; y < 8; y++){
            tiles[x][y]->setEnabled(false);
            }
    }
    loop.exec();
}

void Board::saveMove(Tile* from, Tile* to, pove& move)
{
    move.first = { from, from->piece_color, from->piece_name };
    move.second = { to, to->piece_color, to->piece_name };
}

void Board::revertMove(pove& move)
{
    virtu from = move.first;
    virtu to = move.second;
    from.tile->piece_name = from.name;
    from.tile->piece_color = from.color;
    to.tile->piece_name = to.name;
    to.tile->piece_color = to.color;
    move = {};
}

void Board::moveVirtually(Tile* from, Tile* to)
{
    saveMove(from, to, virtual_move);
    to->piece_color = from->piece_color;
    to->piece_name = from->piece_name;
    from->piece_name = 'e';
}

void Board::moveNormally(Tile* from, Tile* to)
{
    valid.hideValid();
    saveMove(from, to, last_move); // should be used before moving
    to->setPiece(from->piece_name, from->piece_color);
    from->setPiece('e', 0);
}

void Board::castleKing(Tile* king, Tile* destination, Tile* rook)
{
    valid.hideValid();
    moveNormally(king, destination);
    int k = destination->coord.x - king->coord.x > 0 ? -1 : 1; 
    // the rook is always on the left or tight side of king after castling
    int x = destination->coord.x + k;
    int y = destination->coord.y;
    moveNormally(rook, tiles[x][y]);
}

void Board::passPawn(Tile* from, Tile* to)
{
    moveNormally(from, to);
    tiles[to->coord.x][from->coord.y]->setPiece('e', 0);
}

void Board::promotePawn(Tile* tile)
{
    last_move.second.tile->setPiece(tile->piece_name, tile->piece_color);
    for (int i = 0; i < 4; i++) {
        menu[i]->~Tile();
        menu[i] = 0;
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            tiles[x][y]->setEnabled(true);
        }
    }
    emit promotionEnd();
}




