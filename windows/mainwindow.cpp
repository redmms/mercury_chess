#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\game\board.h"
#include "..\game\local_types.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();

    avatar_effect->setBlurRadius(20);
    avatar_effect->setOffset(0, 0);
    avatar_effect->setColor(Qt::green);
    ui->user_avatar->setGraphicsEffect(avatar_effect);

    QPixmap image(":\profile");
    QPixmap  pix(100,100);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(0,0,100,100,14,14);
    QBitmap map = pix.createMaskFromColor(Qt::transparent);  // Создать маску изображения
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
        case stalemate:
            showStatus("Draw by stalemate");
        break;
        case white_wins:
            showStatus("White wins by checkmate");
        break;
        case black_wins:
            showStatus("Black wins by checkmate");
        break;
        case white_resignation:
            showStatus("Black wins by white's resignation");
            // FIX: do something else;
    }
}

void MainWindow::statusSlot(setatus status){
    switch(status){
        case check:
            showStatus("Check! Protect His Majesty!");
        break;
        case new_turn:
            if (board->turn){
                showStatus("White's turn");
                ui->user_avatar->setGraphicsEffect(avatar_effect);
                ui->opponent_avatar->setGraphicsEffect(0);
            }
            else{
                showStatus("Black's turn");
                ui->opponent_avatar->setGraphicsEffect(avatar_effect);
                ui->user_avatar->setGraphicsEffect(0);
            }
        break;
        case invalid_move:
            showStatus("Invalid move");
    }
}

void MainWindow::showStatus(QString status){
    ui->statusBar->showMessage(status, 0);
}

void MainWindow::on_draw_button_clicked()
{
    // send some signal to the opponent's computer, that will be received by some slot
    // that will open a dialog window : draw or not;
    // but that window will stop time for 3 seconds;
}


void MainWindow::on_resign_button_clicked()
{
    endSlot(endnum::white_resignation);
}

