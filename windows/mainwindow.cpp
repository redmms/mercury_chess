#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\game\board.h"
#include "..\game\local_types.h"

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
