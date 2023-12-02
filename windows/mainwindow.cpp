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
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    last_tab(ui->pre_tab)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentWidget(ui->pre_tab);
    for (int i = 0, tab_count = ui->tabWidget->count(); i < tab_count; i++)
        ui->tabWidget->widget(i)->setStyleSheet("QTabWidget::tab > QWidget > QWidget{background: #75752d;}");
    // fixes the background color for tabs. There's some bug in Designer

    std::srand(std::time(nullptr));

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

    QSettings::setDefaultFormat(QSettings::IniFormat); // personal preference
   // settings.beginGroup("names");
    settings.setValue("user_name", "Lazy" +
                      QString::number(std::rand() % (int) std::pow(10, max_nickname_length - 4)));
    settings.setValue("opp_name", "Player2");
    settings.setValue("time_setup", 0);
  //  settings.endGroup();

    avatar_effect->setBlurRadius(20);
    avatar_effect->setOffset(0, 0);
    avatar_effect->setColor(Qt::green);

    auto mask_size = ui->user_avatar->width();
    QPixmap  pix(mask_size,mask_size); // initialize a mask for avatar's rounded corners
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(0,0,mask_size,mask_size,14,14);
    pic_mask = pix.createMaskFromColor(Qt::transparent);

    ui->user_avatar->setMask(pic_mask);
    ui->user_name->setText(settings.value("user_name").toString());
    ui->opponent_avatar->setMask(pic_mask);
    ui->opponent_name->setText(settings.value("opp_name").toString());
    ui->profile_avatar->setMask(pic_mask); // picture in the settings
    ui->profile_avatar->setPixmap(user_pic);
    ui->profile_name->setText(settings.value("user_name").toString());

    auto layout = ui->time_limits_layout;
    QPushButton* button;
    for (int i = 1; i < 10; i++){
        button = qobject_cast<QPushButton*>(layout->itemAt(i)->widget());
        QObject::connect(button, &QPushButton::clicked, [this, button](){
            int minutes_n = button->objectName().mid(3).toInt();
            int time_limit = minutes_n*60000;
            settings.setValue("time_setup", time_limit);
        });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startGame(bool side, int time) // side true for user - white
{
    if (board != nullptr)
        board = new Board(board/*, side*/);
    else
        board = new Board(ui->board_background/*, side*/);

    if (clock != nullptr){
        clock->~QObject();
        //clock->~QObject();
        clock = nullptr;
    }

    ui->user_avatar->setPixmap(user_pic);
    ui->opponent_avatar->setPixmap(opp_pic);
    last_tab = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->game_tab);

    if (side){
        ui->user_avatar->setGraphicsEffect(avatar_effect);
        clock = new ChessClock(this, ui->opponent_timer, ui->user_timer, time);
    }
    else{
        ui->opponent_avatar->setGraphicsEffect(avatar_effect);
        clock = new ChessClock(this, ui->user_timer, ui->opponent_timer, time);
    }

    connect(board, &Board::newStatus, this, &MainWindow::statusSlot);
    connect(board, &Board::theEnd, this, &MainWindow::endSlot);

    connect(this, &MainWindow::timeToSwitchTime, clock, &ChessClock::switchTimer);
    connect(clock, &ChessClock::blackOut, [this](){
        endSlot(endnum::black_out_of_time);
    });
    connect(clock, &ChessClock::whiteOut, [this](){
        endSlot(endnum::white_out_of_time);
    });

    connect(ui->resign_button, &QPushButton::clicked, this, &MainWindow::on_resign_button_clicked);
    connect(ui->draw_button, &QPushButton::clicked, this, &MainWindow::on_draw_button_clicked);

    showStatus("Ready? Go!");
    clock->startTimer();
}

void MainWindow::endSlot(endnum end_type)  // FIX: white_wins and black_wins enum values should
// be changed to user_wins and opp_wins, and what color user plays should be checked in Board::reactOnClick();
{
    QString opp_name = settings.value("opp_name").toString();
    QString info_message;
    switch(end_type){
        case endnum::draw:
            sounds["draw"]->play();
            info_message = "Draw by agreement";
        break;
        case endnum::stalemate:
            sounds["draw"]->play();
            info_message = "Draw by stalemate";
        break;
        case endnum::white_wins:
            sounds["win"]->play();
            info_message = "You win by checkmate";
        break;
        case endnum::black_wins:
            sounds["lose"]->play();
            info_message = opp_name + " wins by checkmate";
        break;
        case endnum::white_resignation:
            sounds["lose"]->play();
            info_message = opp_name + " wins by your resignation";
        break;
        case endnum::white_out_of_time:
            if (match_side){
                sounds["lose"]->play();
                info_message = opp_name  + " wins by your timeout";
            }
            else{
                sounds["win"]->play();
                info_message = "You win by " + opp_name + "'s timeout";
            }
        break;
        case endnum::black_out_of_time:
            if (!match_side){
                sounds["lose"]->play();
                info_message = opp_name  + " wins by your timeout";
            }
            else{
                sounds["win"]->play();
                info_message = "You win by " + opp_name + "'s timeout";
            }
    }
    showStatus(info_message);
    board->setEnabled(false);
    ui->draw_button->disconnect();
    ui->resign_button->disconnect();
    clock->~QObject();
    clock = nullptr;
    QMessageBox msg_box;
    msg_box.setWindowTitle("The end");
    msg_box.setText(info_message);
    msg_box.setIcon(QMessageBox::Information);
    msg_box.addButton("Ok", QMessageBox::AcceptRole);
    msg_box.addButton("Back to the main menu", QMessageBox::RejectRole);
    connect(&msg_box, &QMessageBox::rejected, [this](){
        last_tab = ui->tabWidget->currentWidget();
        ui->tabWidget->setCurrentWidget(ui->friend_connect_tab);
    });
    msg_box.exec();
}

void MainWindow::statusSlot(tatus status)
{
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
        case tatus::eaten_by_user:
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
    emit timeToSwitchTime();
}

void MainWindow::showStatus(const QString& status){
    ui->statusBar->showMessage(status, 0);
}

void MainWindow::switchGlow() // FIX: should be changed for different sides
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
    statusSlot(tatus::draw_suggestion);
}

void MainWindow::on_resign_button_clicked()
{
    endSlot(endnum::white_resignation);
}

void MainWindow::on_actionProfile_triggered()
{
    last_tab = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->settings_tab);
}

void MainWindow::on_change_photo_button_clicked()
{
    // open dialog window to choose a photo
    QString avatar_address = QFileDialog::getOpenFileName(this, "Open File",
                                                      QString("Choose a photo for avatar. Avatar picture will be square at least 95x95 pixel picture."),
                                                      tr("Images (*.png *.jpg *.jpeg *.pgm)"));
    QSize user_size = ui->user_avatar->size();
    user_pic = QPixmap(avatar_address).scaled(user_size);
    ui->user_avatar->setPixmap(user_pic);
    ui->profile_avatar->setPixmap(user_pic);
}

void MainWindow::on_change_name_button_clicked()
{
    QString new_name = ui->name_edit->text();
    ui->name_edit->clear();
    if (new_name.size() <= max_nickname_length) {
        settings.setValue("user_name", new_name);
        ui->user_name->setText(new_name);
        ui->profile_name->setText(new_name);
    } else {
        QMessageBox msg_box;
        msg_box.setWindowTitle("So huge!");
        msg_box.setText("This nickname is too long. Maximum length is " +
                        QString::number(max_nickname_length) );
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
    }
//    QMessageBox msgBox;
//    msgBox.setWindowTitle("Notification");
//    msgBox.setText("Nickname has been changed");
//    msgBox.setIcon(QMessageBox::Information);
//    msgBox.addButton("Ok", QMessageBox::AcceptRole);
////    connect(&msgBox, &QMessageBox::accepted, &msgBox, &QMessageBox::close);
//    msgBox.exec();
}

void MainWindow::on_back_from_settings_clicked()
{
    if (last_tab == ui->pre_tab)
        ui->tabWidget->setCurrentWidget(ui->friend_connect_tab);
    else
        ui->tabWidget->setCurrentWidget(last_tab);
    last_tab = ui->settings_tab;
}

void MainWindow::on_actionWith_friend_triggered()
{
    last_tab = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->friend_connect_tab);
}

void MainWindow::on_registrate_button_clicked()
{
    last_tab = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->settings_tab);
}

void MainWindow::on_guest_button_clicked()
{
    last_tab = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->friend_connect_tab);
}

void MainWindow::on_send_invite_button_clicked()
{
    int chosen_time = settings.value("time_setup").toInt();
    if (chosen_time){
        last_tab = ui->tabWidget->currentWidget();
        ui->tabWidget->setCurrentWidget(ui->game_tab);
        startGame(match_side, settings.value("time_setup").toInt());
    }
    else{
        QMessageBox msg_box;
        msg_box.setWindowTitle("Set up match timer");
        msg_box.setText("You need to choose initial time for chess clock.");
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
    }

}

