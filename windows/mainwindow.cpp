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
    last_tab(ui->pre_tab),
    max_message_width((int) (ui->chat_area->maximumWidth() / 3) * 2)
{
    ui->setupUi(this);
    ui->message_edit->installEventFilter(this);
    ui->mainToolBar->hide();
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentWidget(ui->pre_tab);
    for (int i = 0, tab_count = ui->tabWidget->count(); i < tab_count; i++)
        ui->tabWidget->widget(i)->setStyleSheet("QTabWidget::tab > QWidget > QWidget{background: #75752d;}");
    // fixes the background color for tabs. There's some bug in Designer

    std::srand(std::time(nullptr));

    sounds["move"] = new QSoundEffect;
    sounds["move"]->setSource(QUrl::fromLocalFile(":/sounds/move"));
    sounds["user's piece eaten"] = new QSoundEffect;
    sounds["user's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/user_eaten"));
    sounds["user's piece eaten"]->setVolume(0.6);
    sounds["opponent's piece eaten"] = new QSoundEffect;
    sounds["opponent's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/opp_eaten"));
    sounds["opponent's piece eaten"]->setVolume(0.7);
    sounds["castling"] = new QSoundEffect;
    sounds["castling"]->setSource(QUrl::fromLocalFile(":/sounds/castling"));
    sounds["promotion"] = new QSoundEffect;
    sounds["promotion"]->setSource(QUrl::fromLocalFile(":/sounds/promotion"));
    sounds["check to user"] = new QSoundEffect;
    sounds["check to user"]->setSource(QUrl::fromLocalFile(":/sounds/check"));
    sounds["check to user"]->setVolume(0.7);
    sounds["check to opponent"] = new QSoundEffect;
    sounds["check to opponent"]->setSource(QUrl::fromLocalFile(":/sounds/check_to_opp"));
    sounds["check to opponent"]->setVolume(0.6);
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
                      QString::number(std::rand() % (int) std::pow(10, max_nick - 4)));
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
    ui->profile_name->setText(settings.value("user_name").toString());
    ui->profile_avatar->setPixmap(user_pic);

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

    message_layout->setContentsMargins(5, 5, 5, 5);
    message_box->setLayout(message_layout);
    message_box->resize(ui->chat_area->width(), 0);
    //message_box->setMaximumWidth(ui->chat_area->maximumWidth());
   // message_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   // ui->chat_area->viewport()->setStyleSheet("border-radius: 14");
    ui->chat_area->setWidget(message_box);
    ui->chat_area->setWidgetResizable(true);
   // ui->chat_area->setViewportMargins(0, 14, 0, 14);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startGame(bool side, int time) // side true for user - white
{
    ui->user_avatar->setPixmap(user_pic);
    ui->opponent_avatar->setPixmap(opp_pic);
    last_tab = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->game_tab);
    (side ? ui->user_avatar : ui->opponent_avatar)->setGraphicsEffect(avatar_effect);

    board = new Board(board != nullptr ? board : ui->board_background, side);
    // old board will be destroyed inside Board constructor
    connect(board, &Board::newStatus, this, &MainWindow::statusSlot);
    connect(board, &Board::theEnd, this, &MainWindow::endSlot);

    clock = new ChessClock(board, ui->opponent_timer, ui->user_timer, side, time);
    connect(this, &MainWindow::timeToSwitchTime, clock, &ChessClock::switchTimer);
    connect(clock, &ChessClock::userOut, [this](){
        endSlot(endnum::user_out_of_time);
    });
    connect(clock, &ChessClock::opponentOut, [this](){
        endSlot(endnum::opponent_out_of_time);
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
        case endnum::draw_by_agreement:
            sounds["draw"]->play();
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
            info_message = opp_name  + " wins by your timeout";
        break;
        case endnum::opponent_out_of_time:
            sounds["win"]->play();
            info_message = "You win by " + opp_name + "'s timeout";
    }
    showStatus(info_message);

    clock->stopTimer();
    board->setEnabled(false);
    ui->draw_button->disconnect();
    ui->resign_button->disconnect();

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
        case tatus::check_to_user:
            sounds["check to user"]->play();
            showStatus("Check! Protect His Majesty!");
        break;
        case tatus::check_to_opponent:
            sounds["check to opponent"]->play();
            showStatus("You are a fearless person!");
        break;
        case tatus::users_piece_eaten:
            sounds["user's piece eaten"]->play();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::opponents_piece_eaten:
            sounds["opponent's piece eaten"]->play();
            showStatus(board->turn ? "White's turn" : "Black's turn");
        break;
        case tatus::just_new_turn:
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

void MainWindow::printMessage(QString name, bool own, QString text)
{
    QChar ch;
    for (int cur_len = 0, pos = 0; pos < text.size(); pos++){
        ch = text[pos];
        if (ch == ' ')
            cur_len = 0;
        else{
            cur_len += message_metrics.size(0, ch).width();
            if (cur_len > max_message_width){
                text.insert(pos, "\n");
                cur_len = 0;
            }
        }
    }

    QLabel* message = new QLabel(this);
    message->setStyleSheet("background: gray; border-radius: 14;");
    message->setIndent(7);
    message->setMargin(5);
    message->setMaximumWidth(max_message_width);
    message->setWordWrap(true);
    message->setFont(message_font);
    message->setText(name + "\n" + text);
    message->adjustSize();
    message->setMinimumSize(message->size());
    message->setMaximumSize(message->size());
    // 10 is layout margin here, for shortness
    message_layout->addWidget(message, 0, Qt::AlignTop | (own ? Qt::AlignRight : Qt::AlignLeft));
    message_box->resize(message_box->width(), message_box->height() + message->height() + 10);

    //ui->chat_area->viewport()->update(); // should go BEFORE scrolling scroller
    QTimer::singleShot(200, [&]() {
        auto scroller = ui->chat_area->verticalScrollBar();
        scroller->setValue(scroller->maximum());
    });


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
    endSlot(endnum::user_resignation);
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
    if (new_name.size() <= max_nick) {
        settings.setValue("user_name", new_name);
        ui->user_name->setText(new_name);
        ui->profile_name->setText(new_name);
    } else {
        QMessageBox msg_box;
        msg_box.setWindowTitle("So huge!");
        msg_box.setText("This nickname is too long. Maximum length is " +
                        QString::number(max_nick) );
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
        startGame(true /*bool(std::rand()%2)*/, settings.value("time_setup").toInt());
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

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->message_edit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
        if (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter)
        {
            QString message_text = ui->message_edit->toPlainText();
            printMessage(settings.value("user_name").toString(), true, message_text);
            ui->message_edit->clear();
        }
        else
        {
            return QMainWindow::eventFilter(object, event);
        }
    }
}
