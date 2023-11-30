#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\game\board.h"
#include "..\game\local_types.h"
#include <QPainter>
#include <QGraphicsEffect>
#include <QTabWidget>
#include <QBitmap>
#include <QTabBar>
#include <QFileDialog>
#include <QMessageBox>

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
    sounds["check_to_opp"] = new QSoundEffect;
    sounds["check_to_opp"]->setSource(QUrl::fromLocalFile(":/sounds/check_to_opp"));
    sounds["check_to_opp"]->setVolume(0.7);
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

    settings->setValue("user_name", "Player1");
    settings->setValue("opp_name", "Player2");

    ui->setupUi(this);
    ui->mainToolBar->hide();
    ui->tabWidget->tabBar()->hide();
    for (int i = 0, tab_count = ui->tabWidget->count(); i < tab_count; i++)
        ui->tabWidget->widget(i)->setStyleSheet("QTabWidget::tab > QWidget > QWidget{background: #75752d;}");
    // fix the background color for tabs. There's some bug in Designer

    avatar_effect->setBlurRadius(20);
    avatar_effect->setOffset(0, 0);
    avatar_effect->setColor(Qt::green);
    ui->user_avatar->setGraphicsEffect(avatar_effect);

    auto mask_size = ui->user_avatar->width();
    QPixmap  pix(mask_size,mask_size); // initialize a mask for avatar's rounded corners
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(0,0,mask_size,mask_size,14,14);
    pic_map = pix.createMaskFromColor(Qt::transparent);

    ui->user_avatar->setMask(pic_map);
    ui->user_avatar->setPixmap(user_pic);
    ui->opponent_avatar->setMask(pic_map);
    ui->opponent_avatar->setPixmap(opp_pic);
    ui->profile_avatar->setMask(pic_map); // picture in the settings
    ui->profile_avatar->setPixmap(user_pic);

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
    board->setEnabled(false);
    ui->draw_button->disconnect();
    ui->resign_button->disconnect();
}

void MainWindow::statusSlot(tatus status){

    switch(status){
        case tatus::check:
            if(board->turn){
                sounds["check"]->play();
                showStatus("Check! Protect His Majesty!");
            }
            else{
                sounds["check_to_opp"]->play();
                showStatus("You are a fearless person!");
            }
        break;
        case tatus::eaten_by_opp:
            sounds["eaten by opp"]->play();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus:: eaten_by_user:
            sounds["eaten by user"]->play();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::new_turn:
            sounds["move"]->play();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::invalid_move:
            sounds["invalid move"]->play();
            showStatus("Invalid move");
        return; // will not switch glow effect
        case tatus::castling:
            sounds["castling"]->play();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::promotion:
            sounds["promotion"]->play();
            showStatus(board->turn ? "White's turn" : "Black's turn");
    }
    switchGlow();
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


void MainWindow::on_actionProfile_triggered()
{
    ui->tabWidget->setCurrentIndex(3);
}


void MainWindow::on_change_photo_button_clicked()
{
    // open dialog window to choose a photo
    QString avatar_address = QFileDialog::getOpenFileName(this, "Open File",
                                                      QString("Choose a photo for avatar. Avatar picture will be square at least 95x95 pixel picture."),
                                                      tr("Images (*.png *.jpg *.jpeg *.pgm)"));
    user_pic = QPixmap(avatar_address);
    QSize user_size = ui->user_avatar->size();
    QSize profile_size = ui->profile_avatar->size();
    ui->user_avatar->setPixmap(user_pic.scaled(user_size));
    ui->profile_avatar->setPixmap(user_pic.scaled(profile_size));
}

void MainWindow::on_change_name_button_clicked()
{
    QString new_name = ui->name_edit->text();
    settings->setValue("user_name", new_name);
    ui->name_edit->clear();
    ui->user_name->setText(new_name);
    ui->profile_name->setText(new_name);

    QMessageBox msgBox;
    msgBox.setWindowTitle("Notification");
    msgBox.setText("Nickname has been changed");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton("Ok", QMessageBox::AcceptRole);
//    connect(&msgBox, &QMessageBox::accepted, &msgBox, &QMessageBox::close);
    msgBox.exec();
}


void MainWindow::on_back_from_settings_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_actionWith_friend_triggered()
{
    ui->tabWidget->setCurrentIndex(4);
}

