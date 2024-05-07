#include "mainwindow.h"
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
#include <QClipboard>
#include <QBitmap>
using namespace std;

namespace mmd
{
    MainWindow::MainWindow() :
        QMainWindow(nullptr),
        board{},
        clock{},
        net(new WebClient(this)),
        sounds{},
        avatar_effect(new QGraphicsDropShadowEffect(this)),
        last_tab{},
        game_active(false),
        login_regime(none_r),
        ui(new Ui::MainWindow)
    {
        // .ui file finish strokes
        ui->setupUi(this);

        QLabel* old_history = ui->match_history;
        history_area = (new HistoryArea(this, QColor(111, 196, 81)));
        copyHistoryProp(history_area, old_history);
        replaceOld(history_area, old_history);

        QScrollArea* old_chat = ui->chat_area;
        chat = (new Chat(this, QColor(0, 102, 51)));
        copyChatProp(chat, old_chat);
        replaceOld(chat, old_chat);


        last_tab = ui->pre_tab;
        ui->mainToolBar->hide();
        ui->tabWidget->tabBar()->hide();
        ui->tabWidget->setCurrentWidget(ui->pre_tab);
        for (int i = 0, tab_count = ui->tabWidget->count(); i < tab_count; ++i)
            ui->tabWidget->widget(i)->setStyleSheet("QTabWidget::tab > QWidget > QWidget{background-image: url(:/images/background);}"); //background-color: #75752d;
        // fixes the background color for tabs. There's some bug in Designer
        ui->statusBar->hide();
        ui->draw_button->disconnect();
        ui->resign_button->disconnect();
        ui->menuOnline->setEnabled(false);
        ui->actionWith_AI->setEnabled(false);
        ui->actionRandomly->setEnabled(false);
        this->setWindowIcon(QIcon(":/images/app_icon"));

        srand(time(0));

        // sounds init
        sounds[move_s] = (new QSoundEffect(this));
        sounds[move_s]->setSource(QUrl::fromLocalFile(":/sounds/move"));
        sounds[user_eaten_s] = (new QSoundEffect(this));
        sounds[user_eaten_s]->setSource(QUrl::fromLocalFile(":/sounds/user_eaten"));
        sounds[user_eaten_s]->setVolume(0.6f);
        sounds[opp_eaten_s] = (new QSoundEffect(this));
        sounds[opp_eaten_s]->setSource(QUrl::fromLocalFile(":/sounds/opp_eaten"));
        sounds[opp_eaten_s]->setVolume(0.7f);
        sounds[castling_s] = (new QSoundEffect(this));
        sounds[castling_s]->setSource(QUrl::fromLocalFile(":/sounds/castling"));
        sounds[promotion_s] = (new QSoundEffect(this));
        sounds[promotion_s]->setSource(QUrl::fromLocalFile(":/sounds/promotion"));
        sounds[check_to_user_s] = (new QSoundEffect(this));
        sounds[check_to_user_s]->setSource(QUrl::fromLocalFile(":/sounds/check"));
        sounds[check_to_user_s]->setVolume(0.7f);
        sounds[check_to_opp_s] = (new QSoundEffect(this));
        sounds[check_to_opp_s]->setSource(QUrl::fromLocalFile(":/sounds/check_to_opp"));
        sounds[check_to_opp_s]->setVolume(0.3f);
        sounds[invalid_move_s] = (new QSoundEffect(this));
        sounds[invalid_move_s]->setSource(QUrl::fromLocalFile(":/sounds/invalid"));
        sounds[lose] = (new QSoundEffect(this));
        sounds[lose]->setSource(QUrl::fromLocalFile(":/sounds/lose"));
        sounds[lose]->setVolume(0.5f);
        sounds[win] = (new QSoundEffect(this));
        sounds[win]->setSource(QUrl::fromLocalFile(":/sounds/win"));
        sounds[win]->setVolume(0.8f);
        sounds[draw] = (new QSoundEffect(this));
        sounds[draw]->setSource(QUrl::fromLocalFile(":/sounds/draw"));
        sounds[start] = (new QSoundEffect(this));
        sounds[start]->setSource(QUrl::fromLocalFile(":/sounds/start"));

        // glow effect for avatars
        avatar_effect->setBlurRadius(60);
        avatar_effect->setOffset(0, 0);
        avatar_effect->setColor(QColor("#ffff00"));

        // mask for rounded borders on avatars
        int mask_size = ui->user_avatar->width();
        QPixmap  pix(mask_size, mask_size); // initialize a mask for avatar's rounded corners
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setBrush(Qt::color1); //Qt::black
        painter.drawRoundedRect(0, 0, mask_size, mask_size, 14, 14);
        QBitmap pic_mask = pix.createMaskFromColor(Qt::transparent);

        // settings init
    //settings = QSettings("settings_" + curTime() + ".ini", QSettings::IniFormat);
        settings[user_pass_e] = {};
        settings[user_name_e].setValue("Lazy"_qs +
            QString::number(rand() % int(1e8)));
        settings[opp_name_e].setValue("Friend"_qs);
        settings[time_setup_e].setValue(0);
        settings[match_side_e].setValue(true);
        settings[game_regime_e].setValue(QString(friend_offline));
        settings[def_port_e].setValue(49001);
        settings[port_address_e].setValue(49001);
        settings[def_address_e].setValue("40.113.33.140"_qs);
        settings[ip_address_e].setValue(/*"127.0.0.1"_qs*/"40.113.33.140"_qs);
        settings[max_nick_e].setValue(12);
        settings[pic_w_e].setValue(100);
        settings[pic_h_e].setValue(100);
        setPic(def_pic_e, QPixmap(":images/profile"));
        setPic(user_pic_e, QPixmap(":images/profile"));
        setPic(opp_pic_e, QPixmap(":images/profile"));
        setBMap(pic_mask_e, pic_mask);

        // user and opponent avatars in game and settings
        ui->user_avatar->setMask(getBMap(pic_mask_e));
        ui->opponent_avatar->setMask(getBMap(pic_mask_e));
        ui->profile_avatar->setMask(getBMap(pic_mask_e)); // picture in the settings
        ui->profile_name->setText(settings[user_name_e].toString());
        ui->profile_avatar->setPixmap(QPixmap(":images/profile"));

        // time limit buttons from friend_connect_tab 
        auto layout = ui->time_limits_layout;
        for (int i = 1; i < 10; ++i) {
            QPushButton* button = qobject_cast<QPushButton*>(layout->itemAt(i)->widget());
            if (button)
                connect(button, &QPushButton::clicked, [this, button]() {
                int minutes_n = button->text().toInt(); //button->objectName().mid(3).toInt()
                settings[time_setup_e].setValue(minutes_n);
                    });
        }
    }

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

    void MainWindow::copyBoardProp(Board* young, QLabel* old)
    {
        int size = old->width();
        young->setMinimumSize(size, size);
        young->setMaximumSize(size, size);
        young->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        young->setStyleSheet(young->BoardCss());
    }

    void MainWindow::copyChatProp(Chat* young, QScrollArea* old)
    {
        young->setStyleSheet("QAbstractScrollArea{background: transparent; border: none;}");
        young->setGeometry(old->geometry());
        young->setSizePolicy(old->sizePolicy());
        young->setMinimumSize(old->minimumSize());
        young->setMaximumSize(old->maximumSize());
        young->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        young->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    void MainWindow::copyHistoryProp(HistoryArea* young, QLabel* old)
    {
        young->setStyleSheet("QAbstractScrollArea{background: transparent; border: none;}");
        young->setGeometry(old->geometry());
        young->setSizePolicy(old->sizePolicy());
        young->setMinimumSize(old->minimumSize());
        young->setMaximumSize(old->maximumSize());
        young->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        young->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    void MainWindow::openStopGameBox()
    {
        showBox("Stop active game",
            "Stop your current game at first.",
            QMessageBox::Warning);
    }

    void MainWindow::showStatus(const QString& status)
    {
        ui->statusBar->showMessage(status, 0);
    }

    void MainWindow::switchGlow()
    {
        bool match_side = settings[match_side_e].toBool();
        if (board->Turn() == match_side)
            ui->user_avatar->setGraphicsEffect(avatar_effect);
        else
            ui->opponent_avatar->setGraphicsEffect(avatar_effect);
        this->update();
    }

    int MainWindow::changeLocalName(QString name)
    {
        int max_nick = settings[max_nick_e].toInt();
        if (name.isEmpty()) {
            return 1;
        }
        else if (name.size() > max_nick) {
            return 2;
        }
        else {
            settings[user_name_e].setValue(name);
            ui->user_name->setText(name);
            ui->profile_name->setText(name);
        }
        return 0;
    }

    void MainWindow::startGame(QString game_regime) // side true for user - white
    {
        if (game_active) {
            if (settings[game_regime_e].toString() == friend_online && net) {
                net->sendToServer(packnum::interrupt_signal);
            }
            endSlot(endnum::interrupt);
        }
        settings[game_regime_e].setValue(game_regime);
        if (game_regime == friend_online) {
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
        else if (game_regime == friend_offline || game_regime == training) {
            if (game_regime == training) {
                settings[opp_name_e].setValue("DarkSide"_qs);
            }
            settings[match_side_e].setValue(true);
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
        else if (game_regime == historical) {
            setPic(opp_pic_e, getPic(def_pic_e));
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
        ui->user_avatar->setPixmap(getPic(user_pic_e));
        ui->user_name->setText(settings[user_name_e].toString());
        ui->opponent_avatar->setPixmap(getPic(opp_pic_e));
        ui->opponent_name->setText(settings[opp_name_e].toString());
        chat->clearMessages();
        ui->statusBar->show();
        history_area->clearStory();

        if (board) {
            Board* old_board = board;
            board = (new Board(this, old_board->width()));
            copyBoardProp(board, old_board);
            replaceOld(board, old_board);
        }
        else if (ui->board_background) {
            QLabel* old_board = ui->board_background;
            board = (new Board(this, old_board->width()));
            copyBoardProp(board, old_board);
            replaceOld(board, old_board);
        }
        else {
            qDebug() << "ERROR: in MainWindow::startGame() with board/ui->background pointer";
        }

        connect(board, &Board::newStatus, this, &MainWindow::statusSlot);
        connect(board, &Board::theEnd, this, &MainWindow::endSlot);

        bool match_side = settings[match_side_e].toBool();
        (match_side == board->Turn() ? ui->user_avatar : ui->opponent_avatar)->setGraphicsEffect(avatar_effect);

        if (game_regime == friend_online) {
            connect(board, &Board::moveMade, [this](scoord from, scoord to, char promotion_type) {
                net->sendToServer(packnum::move_pack, {}, {}, from, to, promotion_type);
                });

            int time = settings[time_setup_e].toInt();
            clock = (new ChessClock(board, ui->opponent_timer, ui->user_timer, match_side, time));
            // old clock will be destroyed with an old board as a child
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
        sounds[start]->play();
    }

    void MainWindow::endSlot(endnum end_type)
    {
        if (!game_active) {
            qDebug() << "Application tried to close inactive game";
            return;
        }

        if (clock)
            clock->stopTimer();

        QString game_regime = settings[game_regime_e].toString();
        if (game_regime != historical) {
            board->setEnabled(false);
            ui->draw_button->disconnect();
            ui->resign_button->disconnect();
            ui->actionProfile->setEnabled(true);
            disconnect(this, &MainWindow::editReturnPressed, this, &MainWindow::editReturnSlot);
            ui->message_edit->removeEventFilter(this);
            game_active = false;
            ui->statusBar->hide();
            board->setEndType(end_type);
        }

        QString opp_name = settings[opp_name_e].toString();
        QString info_message;
        auto icon_type = QMessageBox::Information;
        switch (end_type) {
        case endnum::draw_by_agreement:
            sounds[draw]->play();
            info_message = "Draw by agreement";
            break;
        case endnum::draw_by_stalemate:
            sounds[draw]->play();
            info_message = "Draw by stalemate";
            break;
        case endnum::user_wins:
            sounds[win]->play();
            info_message = "You win by checkmate";
            break;
        case endnum::opponent_wins:
            sounds[lose]->play();
            info_message = opp_name + " wins by checkmate";
            break;
        case endnum::user_resignation:
            sounds[lose]->play();
            info_message = opp_name + " wins by your resignation";
            break;
        case endnum::opponent_resignation:
            sounds[win]->play();
            info_message = "You win by " + opp_name + "'s resignation";
            break;
        case endnum::user_out_of_time:
            sounds[lose]->play();
            info_message = opp_name + " wins by your timeout";
            break;
        case endnum::opponent_out_of_time:
            sounds[win]->play();
            info_message = "You win by " + opp_name + "'s timeout";
            break;
        case endnum::opponent_disconnected_end:
            icon_type = QMessageBox::Critical;
            info_message = opp_name + " disconnected. Maybe he doesn't like to play with you?";
            break;
        case endnum::server_disconnected:
            info_message = "-1";
            return;
        case endnum::interrupt:
            info_message = "-1";
            return;
        }

        if (game_regime == friend_online && net) {
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

    void MainWindow::statusSlot(estatus status)
    {
        QString user = "Your turn";
        QString opp = settings[opp_name_e].toString() + "'s turn";
        switch (status) {
        case estatus::check_to_user:
            sounds[check_to_user_s]->play();
            showStatus("Check! Protect His Majesty!");
            break;
        case estatus::check_to_opponent:
            sounds[check_to_opp_s]->play();
            showStatus("You are a fearless person!");
            break;
        case estatus::user_piece_eaten:
            sounds[user_eaten_s]->play();
            showStatus(board->Turn() ? user : opp);
            break;
        case estatus::opponent_piece_eaten:
            sounds[opp_eaten_s]->play();
            showStatus(board->Turn() ? user : opp);
            break;
        case estatus::just_new_turn:
            sounds[move_s]->play();
            showStatus(board->Turn() ? user : opp);
            break;
        case estatus::invalid_move:
            sounds[invalid_move_s]->play();
            showStatus("Invalid move");
            return; // will not switch glow effect
        case estatus::castling:
            sounds[castling_s]->play();
            showStatus(board->Turn() ? user : opp);
            break;
        case estatus::promotion:
            sounds[promotion_s]->play();
            showStatus(board->Turn() ? user : opp);
            break;
        }
        switchGlow();
        emit timeToSwitchTime();
        size_t order = board->History().size();
        if (order) {
            halfmove last_move = board->History().back();
            QString game_regime = settings[game_regime_e].toString();
            if (game_regime != historical) {
                history_area->writeStory(order, last_move);
            }
        }
        else {
            qDebug() << "WARNING: you tried to write an empty history";
        }
    }
}  // namespace mmd