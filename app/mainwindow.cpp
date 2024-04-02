#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "webclient.h"
#include "offline_dialog.h"
#include "rules_dialog.h"
#include "history_area.h"
#include "chat.h"
#include "fen_dialog.h"
#include "../game/board.h"
#include "../game/clock.h"
#include "../archiver/archiver.h"
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include <QTabBar>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QClipboard>
using namespace std;

MainWindow::MainWindow(QString app_dir_) :
    QMainWindow(nullptr),
    app_dir(app_dir_),
    board{},
    clock{},
    net(new WebClient(this)),
    sounds{},
    avatar_effect(new QGraphicsDropShadowEffect(this)),
    last_tab{},
    game_active(false),
    login_regime(0),
    ui(new Ui::MainWindow)
{
    // .ui file finish strokes
    ui->setupUi(this);
    history_area = (new HistoryArea(this, ui->match_history, QColor(111, 196, 81)));
    chat = (new Chat(this, ui->chat_area, QColor(0, 102, 51)));
    last_tab = ui->pre_tab;
    ui->mainToolBar->hide();
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentWidget(ui->pre_tab);
    for (int i = 0, tab_count = ui->tabWidget->count(); i < tab_count; i++)
        ui->tabWidget->widget(i)->setStyleSheet("QTabWidget::tab > QWidget > QWidget{background-image: url(:/images/background);}"); //background-color: #75752d;
    // fixes the background color for tabs. There's some bug in Designer
    ui->statusBar->hide();
    ui->draw_button->disconnect();
    ui->resign_button->disconnect();
    ui->menuOnline->setEnabled(false);
    ui->actionWith_AI->setEnabled(false);
    //ui->actionTraining->setEnabled(false);
    ui->actionRandomly->setEnabled(false);
    //ui->actionProfile->setEnabled(false);
    this->setWindowIcon(QIcon(":/images/app_icon"));

    srand(time(0));

    // sounds init
    sounds["move"]=(new QSoundEffect);
    sounds["move"]->setSource(QUrl::fromLocalFile(":/sounds/move"));
    sounds["user's piece eaten"]=(new QSoundEffect);
    sounds["user's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/user_eaten"));
    sounds["user's piece eaten"]->setVolume(0.6f);
    sounds["opponent's piece eaten"]=(new QSoundEffect);
    sounds["opponent's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/opp_eaten"));
    sounds["opponent's piece eaten"]->setVolume(0.7f);
    sounds["castling"]=(new QSoundEffect);
    sounds["castling"]->setSource(QUrl::fromLocalFile(":/sounds/castling"));
    sounds["promotion"]=(new QSoundEffect);
    sounds["promotion"]->setSource(QUrl::fromLocalFile(":/sounds/promotion"));
    sounds["check to user"]=(new QSoundEffect);
    sounds["check to user"]->setSource(QUrl::fromLocalFile(":/sounds/check"));
    sounds["check to user"]->setVolume(0.7f);
    sounds["check to opponent"]=(new QSoundEffect);
    sounds["check to opponent"]->setSource(QUrl::fromLocalFile(":/sounds/check_to_opp"));
    sounds["check to opponent"]->setVolume(0.3f);
    sounds["invalid move"]=(new QSoundEffect);
    sounds["invalid move"]->setSource(QUrl::fromLocalFile(":/sounds/invalid"));
    sounds["lose"]=(new QSoundEffect);
    sounds["lose"]->setSource(QUrl::fromLocalFile(":/sounds/lose"));
    sounds["lose"]->setVolume(0.5f);
    sounds["win"]=(new QSoundEffect);
    sounds["win"]->setSource(QUrl::fromLocalFile(":/sounds/win"));
    sounds["win"]->setVolume(0.8f);
    sounds["draw"]=(new QSoundEffect);
    sounds["draw"]->setSource(QUrl::fromLocalFile(":/sounds/draw"));
    sounds["start"]=(new QSoundEffect);
    sounds["start"]->setSource(QUrl::fromLocalFile(":/sounds/start"));

    // glow effect for avatars
    avatar_effect->setBlurRadius(40);
    avatar_effect->setOffset(0, 0);
    avatar_effect->setColor(Qt::green);

    // mask for rounded borders on avatars
    auto mask_size = ui->user_avatar->width();
    QPixmap  pix(mask_size, mask_size); // initialize a mask for avatar's rounded corners
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setBrush(Qt::color1); //Qt::black
    painter.drawRoundedRect(0, 0, mask_size, mask_size, 14, 14);
    QBitmap pic_mask = pix.createMaskFromColor(Qt::transparent);

    // settings init
//settings = QSettings("settings_" + curTime() + ".ini", QSettings::IniFormat);
    settings["user_name"].setValue(QString("Lazy") +
        QString::number(rand() % (int)pow(10, 8)));
    settings["opp_name"].setValue(QString("Player2"));
    settings["time_setup"].setValue(0);
    settings["match_side"].setValue(true);
    settings["game_regime"].setValue(QString("friend_offline"));
    settings["def_port"].setValue(49001);
    settings["port_address"].setValue(49001);
    settings["def_address"].setValue(/*"127.0.0.1"*/(QString)"40.113.33.140");
    settings["ip_address"].setValue((QString)"127.0.0.1"/*(QString)"40.113.33.140"*/);
    settings["max_nick"].setValue(12);
    settings["pic_w"].setValue(100);
    settings["pic_h"].setValue(100);
    setPic("def_pic", QPixmap(":images/profile"));
    setPic("user_pic", QPixmap(":images/profile"));
    setPic("opp_pic", QPixmap(":images/profile"));
    setBMap("pic_mask", pic_mask);

    // user and opponent avatars in game and settings
    ui->user_avatar->setMask(getBMap("pic_mask"));
    ui->opponent_avatar->setMask(getBMap("pic_mask"));
    ui->profile_avatar->setMask(getBMap("pic_mask")); // picture in the settings
    ui->profile_name->setText(settings["user_name"].toString());
    ui->profile_avatar->setPixmap(QPixmap(":images/profile"));

    // time limit buttons from friend_connect_tab 
    auto layout = ui->time_limits_layout;
    QPushButton* button;
    for (int i = 1; i < 10; i++) {
        button = qobject_cast<QPushButton*>(layout->itemAt(i)->widget());
        if (button)
            connect(button, &QPushButton::clicked, [this, button]() {
                int minutes_n = button->text().toInt(); //button->objectName().mid(3).toInt()
                settings["time_setup"].setValue(minutes_n);
            });
    }
}

MainWindow::~MainWindow() {
    //QFile file(settings.fileName());
    //if (file.exists()) {
    //    file.remove();
    //}
    delete ui;
}

#include "mainwindow_buttons.hpp"

void MainWindow::openTab(QWidget* page)
{
    if (!ui || !ui->tabWidget || !page)
     {
        qWarning() << "ERROR: in open tab with pointers";
        return;
     }
    last_tab = ui->tabWidget->currentWidget();
    if (!last_tab)
     {
        qWarning() << "ERROR: in open tab with last_tab pointer";
        return;
     }
    ui->tabWidget->setCurrentWidget(page);
}

void MainWindow::openStopGameBox()
{
    showBox("Stop active game",
            "Stop your current game at first.",
            QMessageBox::Warning);
}

//void MainWindow::openInDevBox()
//{
//    showBox("Not available yet",
//            "This function hasn't been developed yet, but you can donate money to speed up the process.");
//}

void MainWindow::showStatus(const QString& status) 
{
    ui->statusBar->showMessage(status, 0);
}

void MainWindow::switchGlow()
{
    bool match_side = settings["match_side"].toBool();
    if (board->turn == match_side)
        ui->user_avatar->setGraphicsEffect(avatar_effect);
    else
        ui->opponent_avatar->setGraphicsEffect(avatar_effect);
    this->update();
}

int MainWindow::changeLocalName(QString name)
{
    int max_nick = settings["max_nick"].toInt();
    if (name.isEmpty()) {
        return 1;
    }
    else if (name.size() > max_nick) {
        return 2;
    }
    else {
        settings["user_name"].setValue(name);
        ui->user_name->setText(name);
        ui->profile_name->setText(name);
    }
    return 0;
}

void MainWindow::startGame(QString game_regime) // side true for user - white
{
    if (game_active) {
        if (settings["game_regime"].toString() == "friend_online" && net) {
            net->sendToServer(packnum::interrupt_signal);
        }
        endSlot(endnum::interrupt);
    }
    settings["game_regime"].setValue(game_regime);
    if (game_regime == "friend_online"){
        ui->actionProfile->setEnabled(false);
        ui->message_edit->setPlainText("Great move! Have you studied in a clown school?");
        ui->message_edit->installEventFilter(this);
        bool messages_connected = connect(this, &MainWindow::editReturnPressed, this, &MainWindow::editReturnSlot);
        ui->resign_button->setText("Resign");
        ui->draw_button->setText("Suggest a draw");
        ui->resign_button->disconnect();
        ui->draw_button->disconnect();
        connect(ui->resign_button, &QPushButton::clicked, this, &MainWindow::on_resign_button_clicked);
        connect(ui->draw_button, &QPushButton::clicked, this, &MainWindow::on_draw_button_clicked);
        if (!messages_connected) {
            qDebug() << "Messages were not connected";
        }
        qDebug() << "editReturnPressed receivers number is" << receivers(SIGNAL(editReturnPressed));
    }
    else if (game_regime == "friend_offline" || game_regime == "training") {
        settings["match_side"].setValue(true);
        ui->message_edit->setPlainText("Chat is off. But you can chat with yourself if you are a hikikomori.");
        ui->user_timer->setText("");
        ui->opponent_timer->setText("");
        ui->resign_button->setText("Previous menu");
        ui->draw_button->setText("Stop game");
        ui->resign_button->disconnect();
        ui->draw_button->disconnect();
        connect(ui->resign_button, &QPushButton::clicked, this, &MainWindow::my_offline_back_button_clicked);
        connect(ui->draw_button, &QPushButton::clicked, this, &MainWindow::my_offline_stop_button_clicked);
    }
    else if (game_regime == "history") {
        setPic("opp_pic", getPic("def_pic"));
        ui->message_edit->setPlainText("Chat is off. But you can chat with yourself if you are a hikikomori."); 
        ui->user_timer->setText("");
        ui->opponent_timer->setText("");
        ui->resign_button->setText("Forward");
        ui->draw_button->setText("Back");
        ui->resign_button->disconnect();
        ui->draw_button->disconnect();
        connect(ui->resign_button, &QPushButton::clicked, this, &MainWindow::my_history_next_button_clicked);
        connect(ui->draw_button, &QPushButton::clicked, this, &MainWindow::my_history_previous_button_clicked);
    }
    openTab(ui->game_tab);
    activateWindow();
    ui->user_avatar->setPixmap(getPic("user_pic"));
    ui->user_name->setText(settings["user_name"].toString());
    ui->opponent_avatar->setPixmap(getPic("opp_pic"));
    ui->opponent_name->setText(settings["opp_name"].toString());
    bool match_side = settings["match_side"].toBool();
    (match_side ? ui->user_avatar : ui->opponent_avatar)->setGraphicsEffect(avatar_effect);
    chat->clearMessages();
    ui->statusBar->show();
    history_area->clearStory();

    if (board){
        Board* old_board = board;
        board = (new Board(this, old_board));
        delete old_board;
    }
    else if (ui->board_background){
        board = (new Board(this, ui->board_background));
        ui->board_background->~QLabel();
    }
    else{
        qDebug() << "ERROR: in MainWindow::startGame() with board/ui->background pointer";
    }
    // old board will be destroyed inside Board constructor

    connect(board, &Board::newStatus, this, &MainWindow::statusSlot);
    connect(board, &Board::theEnd, this, &MainWindow::endSlot);

    if (game_regime == "friend_online"){
        connect(board, &Board::moveMade, [this](scoord from, scoord to, char promotion_type) {
            net->sendToServer(packnum::move, {}, {}, from, to, promotion_type);
        });

        int time = settings["time_setup"].toInt();
        clock=(new ChessClock(board, ui->opponent_timer, ui->user_timer, match_side, time));
        // old clock will be destroyed inside Board constructor as a child // FIX: at least it should be
        connect(this, &MainWindow::timeToSwitchTime, clock, &ChessClock::switchTimer);
        connect(clock, &ChessClock::userOut, [this]() {
            endSlot(endnum::user_out_of_time);
        });
        connect(clock, &ChessClock::opponentOut, [this]() {
            endSlot(endnum::opponent_out_of_time);
        });
       
        clock->startTimer();
    }

    showStatus("Ready? Go!");
    game_active = true;
    sounds["start"]->play();
}

void MainWindow::endSlot(endnum end_type)
{
    if (!game_active){
        qDebug() << "Application tried to close inactive game";
        return;
    }
    if (clock)
        clock->stopTimer();
    board->setEnabled(false);
    ui->draw_button->disconnect();
    ui->resign_button->disconnect();
    ui->actionProfile->setEnabled(true);
    disconnect(this, &MainWindow::editReturnPressed, this, &MainWindow::editReturnSlot);
    ui->message_edit->removeEventFilter(this);
    game_active = false;
    ui->statusBar->hide();
    board->end_type = end_type;


    QString opp_name = settings["opp_name"].toString();
    QString info_message;
    auto icon_type = QMessageBox::Information;
    switch (end_type) {
    case endnum::draw_by_agreement:
        sounds["draw"]->play(); // FIX: this may cause server to keep game active (player1 and player2 initialized)
        // because neither opponent neither user will send end_game signal to server
        info_message = "Draw by agreement";
        break;
    case endnum::draw_by_stalemate:
        sounds["draw"]->play();
        info_message = "Draw by stalemate";
        break;
    case endnum::user_wins:
        sounds["win"]->play();
        info_message = "You win by checkmate";
        break;
    case endnum::opponent_wins:
        sounds["lose"]->play();
        info_message = opp_name + " wins by checkmate";
        break;
    case endnum::user_resignation:
        sounds["lose"]->play();
        info_message = opp_name + " wins by your resignation";
        break;
    case endnum::opponent_resignation:
        sounds["win"]->play();
        info_message = "You win by " + opp_name + "'s resignation";
        break;
    case endnum::user_out_of_time:
        sounds["lose"]->play();
        info_message = opp_name + " wins by your timeout";
        break;
    case endnum::opponent_out_of_time:
        sounds["win"]->play();
        info_message = "You win by " + opp_name + "'s timeout";
        break;
    case endnum::opponent_disconnected_end:
        icon_type = QMessageBox::Critical;
        info_message = "Opponent disconnected. Maybe he doesn't like to play with you?";
        break;
    case endnum::server_disconnected:
        info_message = "-1";
        return;
    case endnum::interrupt:
        info_message = "-1";
        return;
    }

    if (settings["game_regime"].toString() == "friend_online" && net) {
        net->sendToServer(packnum::end_game);
    }
    showStatus(info_message);
    QMessageBox msg_box(this);
    msg_box.setWindowTitle("The end");
    msg_box.setText(info_message);
    msg_box.setIcon(icon_type);
    msg_box.addButton("Ok", QMessageBox::AcceptRole);
    msg_box.addButton("Back to the menu", QMessageBox::RejectRole);
    connect(&msg_box, &QMessageBox::rejected, [this]() {
        openTab(ui->friend_connect_tab);
        });
    msg_box.exec();
}

void MainWindow::statusSlot(tatus status)
{
//    int i = 3;
////    for (int i = 1; i <= 5; i++)
//        qDebug() << "Counted moves:" << board->valid->countMovesTest(i);
    QString user = "Your turn";
    QString opp = settings["opp_name"].toString() + "'s turn";
    switch (status) {
    case tatus::check_to_user:
        sounds["check to user"]->play();
        showStatus("Check! Protect His Majesty!");
        break;
    case tatus::check_to_opponent:
        sounds["check to opponent"]->play();
        showStatus("You are a fearless person!");
        break;
    case tatus::user_piece_eaten:
        sounds["user's piece eaten"]->play();
        showStatus(board->turn ? user : opp);
        break;
    case tatus::opponent_piece_eaten:
        sounds["opponent's piece eaten"]->play();
        showStatus(board->turn ? user : opp);
        break;
    case tatus::just_new_turn:
        sounds["move"]->play();
        showStatus(board->turn ? user : opp);
        break;
    case tatus::invalid_move:
        sounds["invalid move"]->play();
        showStatus("Invalid move");
        return; // will not switch glow effect
    case tatus::castling:
        sounds["castling"]->play();
        showStatus(board->turn ? user : opp);
        break;
    case tatus::promotion:
        sounds["promotion"]->play();
        showStatus(board->turn ? user : opp);
        break;
    }
    switchGlow();
    emit timeToSwitchTime();
    size_t order = board->history.size();
    halfmove last_move = board->history.back();
    QString game_regime = settings["game_regime"].toString();
    if (game_regime != "history") {
        history_area->writeStory(order, last_move);
    }
}