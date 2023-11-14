#pragma once
#include "board.h"
#include <QGridLayout>
#include "local_types.h"

Board::Board(QLabel* background = 0) :
    tile_size(background->width() / 9)
{
    setGeometry(background->geometry());   // replace ui board, by this class
    setStyleSheet("background: rgb(170, 170, 120);");
    background->parentWidget()->layout()->replaceWidget(background, this);
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
    else if (tile != from_tile && tile->piece_color == turn && tile->piece_name != 'e') {
    // if the second click
    // is on the piece of same color then pick it instead
        valid.hideValid();
        from_tile = tile;
        valid.showValid(tile);
    }
    else if (valid.isValid(tile)) { // if it's the second click and move is valid
    //then move pieces
        valid.hideValid();
        tile->setPiece(from_tile->piece_name, from_tile->piece_color);       
        from_tile->setPiece('e', 0); // could be extracted into move(Tile* from, Tile* to), or with scoord from, scoord to
        from_tile = nullptr;
        turn = !turn;
        Tile* king = turn ? white_king : black_king;
        if (valid.inCheck(king)){
            check = true;
            if (valid.inCheckmate(king)){
                if (turn){
                    emit newStatus("Black wins");
                    emit theEnd(endnum::black_wins);
                    return;
                }
                else{
                    emit newStatus("White wins");
                    emit theEnd(endnum::white_wins);
                    return;
                }
            }
            else{
                emit newStatus("Check! Protect His Majesty!");
                return;
                }
        }
        else if(valid.inStalemate(turn)){
            emit newStatus("Draw by stalemate");
            emit theEnd(endnum::stalemate);
            return;
        }
        check = false;
        emit newStatus(turn ? "White's turn" : "Black's turn");
    }
    else{
        emit newStatus("Invalid move");
    }
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
            tiles[x][y]->num = y * 8 + x;
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

void Board::moveVirtually(scoord from, scoord to)
{
    last_move.first = {tiles[from.x][from.y], tiles[from.x][from.y]->piece_color, tiles[from.x][from.y]->piece_name};
    last_move.second = {tiles[to.x][to.y], tiles[to.x][to.y]->piece_color, tiles[to.x][to.y]->piece_name};
    tiles[to.x][to.y]->piece_color = tiles[from.x][from.y]->piece_color;
    tiles[to.x][to.y]->piece_name = tiles[from.x][from.y]->piece_name;
}

void Board::revertLast()
{
    virtu from = last_move.first;
    virtu to = last_move.second;
    from.tile->piece_name = from.name;
    from.tile->piece_color = from.color;
    to.tile->piece_name = to.name;
    to.tile->piece_color = to.color;
    last_move = {};
}
