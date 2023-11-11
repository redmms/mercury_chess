#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\game\board.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();
    Board* board = new Board(ui->board_background);
    QObject::connect(board, &Board::newStatus, this, &MainWindow::changeStatus);
    ui->statusBar->showMessage("Ready? Go!", 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeStatus(QString status){
    ui->statusBar->showMessage(status, 0);
}