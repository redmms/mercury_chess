#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\game\board.h"
#include "..\game\local_types.h"
#include <QPainter>
#include <QGraphicsEffect>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    sounds["move"] = new QSoundEffect;
    sounds["move"]->setSource(QUrl::fromLocalFile(":/sounds/move"));
    sounds["eaten by opp"] = new QSoundEffect;
    sounds["eaten by opp"]->setSource(QUrl::fromLocalFile(":/sounds/user_eaten"));
    sounds["eaten by opp"]->setVolume(0.6);
    sounds["eaten by user"] = new QSoundEffect;
    sounds["eaten by user"]->setSource(QUrl::fromLocalFile(":/sounds/opp_eaten"));
    sounds["eaten by user"]->setVolume(0.7);
    sounds["castling"] = new QSoundEffect;
    sounds["castling"]->setSource(QUrl::fromLocalFile(":/sounds/castling"));
    sounds["promotion"] = new QSoundEffect;
    sounds["promotion"]->setSource(QUrl::fromLocalFile(":/sounds/promotion"));
    sounds["check"] = new QSoundEffect;
    sounds["check"]->setSource(QUrl::fromLocalFile(":/sounds/check"));
    sounds["check"]->setVolume(0.7);
    sounds["invalid move"] = new QSoundEffect;
    sounds["invalid move"]->setSource(QUrl::fromLocalFile(":/sounds/invalid"));
    sounds["lose"] = new QSoundEffect;
    sounds["lose"]->setSource(QUrl::fromLocalFile(":/sounds/lose"));
    sounds["lose"]->setVolume(0.6);
    sounds["win"] = new QSoundEffect;
    sounds["win"]->setSource(QUrl::fromLocalFile(":/sounds/win"));
    sounds["win"]->setVolume(0.8);
    sounds["draw"] = new QSoundEffect;
    sounds["draw"]->setSource(QUrl::fromLocalFile(":/sounds/draw"));

    ui->setupUi(this);
    ui->mainToolBar->hide();

    avatar_effect->setBlurRadius(20);
    avatar_effect->setOffset(0, 0);
    avatar_effect->setColor(Qt::green);
    ui->user_avatar->setGraphicsEffect(avatar_effect);

    QPixmap image(":images/profile"); // round the avatars' corners
    QPixmap  pix(100,100);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(0,0,100,100,14,14);
    QBitmap map = pix.createMaskFromColor(Qt::transparent);
    ui->user_avatar->setMask(map);
    ui->user_avatar->setPixmap(image);
    ui->opponent_avatar->setMask(map);
    ui->opponent_avatar->setPixmap(image);

    board = new Board(ui->board_background);

    QObject::connect(board, &Board::newStatus, this, &MainWindow::statusSlot);
    QObject::connect(board, &Board::theEnd, this, &MainWindow::endSlot);

    showStatus("Ready? Go!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::endSlot(endnum end_type)
{
    switch(end_type){
        case endnum::stalemate:
            sounds["draw"]->play();
            showStatus("Draw by stalemate");
        break;
        case endnum::white_wins:
            sounds["win"]->play();
            showStatus("White wins by checkmate");
        break;
        case endnum::black_wins:
            sounds["lose"]->play();
            showStatus("Black wins by checkmate");
        break;
        case endnum::white_resignation:
            sounds["lose"]->play();
            showStatus("Black wins by white's resignation");
            // FIX: do something else;
    }
}

void MainWindow::statusSlot(tatus status){
    switch(status){
        case tatus::check:
            sounds["check"]->play();
            switchGlow();
            showStatus("Check! Protect His Majesty!");
        break;
        case tatus::eaten_by_opp:
            sounds["eaten by opp"]->play();
            switchGlow();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus:: eaten_by_user:
            sounds["eaten by user"]->play();
            switchGlow();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::new_turn:
            sounds["move"]->play();
            switchGlow();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::invalid_move:
            sounds["invalid move"]->play();
            showStatus("Invalid move");
        break;
        case tatus::castling:
            sounds["castling"]->play();
            switchGlow();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::promotion:
            sounds["promotion"]->play();
            switchGlow();
            showStatus(board->turn ? "White's turn" : "Black's turn");
    }
}

void MainWindow::showStatus(const QString& status){
    ui->statusBar->showMessage(status, 0);
}

void MainWindow::switchGlow()
{
    if (board->turn){
        ui->user_avatar->setGraphicsEffect(avatar_effect);
        this->update();
    }
    else{
        ui->opponent_avatar->setGraphicsEffect(avatar_effect);
        this->update();
    }
}

void MainWindow::on_draw_button_clicked()
{
    // send some signal to the opponent's computer, that will be received by some slot
    // that will open a dialog window : draw or not;
    // but that window will stop chess timer for 3 seconds;
}


void MainWindow::on_resign_button_clicked()
{
    endSlot(endnum::white_resignation);
}

