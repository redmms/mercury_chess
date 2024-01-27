#include "mainwindow.h"
#include "C:\Qt_projects\multicolor_chess\src\game\board.h"
#include "C:\Qt_projects\multicolor_chess\src\game\validation.h"
#include "C:\Qt_projects\multicolor_chess\src\game\local_types.h"
#include "webclient.h"
#include <QPainter>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <QBitmap>
#include <QEventLoop>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QScrollBar>
#include <QSoundEffect>
#include <QTabBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QMetaObject>
#include <QMetaProperty>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QCryptographicHash>
#include <cmath>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
    board{},
    clock{},
	net(new WebClient(this)),
	sounds{},
    settings("settings_" + curTime() + ".ini", QSettings::IniFormat),
    avatar_effect(new QGraphicsDropShadowEffect(this)),
	user_pic(":images/profile"),
	opp_pic(":images/profile"),
    default_pic(":images/profile"),
	pic_mask{},
    last_tab{},
    max_nick(12),
    message_layout(new QVBoxLayout()),
	message_box(new QWidget(this)),
	message_font{ "Segoe Print", 12 },
	message_metrics{ message_font },
    max_message_width{},
    rounded_area(new RoundedScrollArea(this, QColor(0, 102, 51))),
    game_active(false),
    login_regime(0),
    waiting_for_invite_respond(false),
    ui(new Ui::MainWindow),
    default_address("40.113.33.140"),
    default_port(49001),
    history_area(new RoundedScrollArea(this, QColor(111, 196, 81))),
    history_label(new QLabel(this))
{
	// .ui file finish strokes
	ui->setupUi(this);
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
    //ui->actionProfile->setEnabled(false);

    std::srand(std::time(0));

	// sounds init
    sounds["move"].reset(new QSoundEffect);
	sounds["move"]->setSource(QUrl::fromLocalFile(":/sounds/move"));
    sounds["user's piece eaten"].reset(new QSoundEffect);
	sounds["user's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/user_eaten"));
	sounds["user's piece eaten"]->setVolume(0.6f);
    sounds["opponent's piece eaten"].reset(new QSoundEffect);
	sounds["opponent's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/opp_eaten"));
	sounds["opponent's piece eaten"]->setVolume(0.7f);
    sounds["castling"].reset(new QSoundEffect);
	sounds["castling"]->setSource(QUrl::fromLocalFile(":/sounds/castling"));
    sounds["promotion"].reset(new QSoundEffect);
	sounds["promotion"]->setSource(QUrl::fromLocalFile(":/sounds/promotion"));
    sounds["check to user"].reset(new QSoundEffect);
	sounds["check to user"]->setSource(QUrl::fromLocalFile(":/sounds/check"));
	sounds["check to user"]->setVolume(0.7f);
    sounds["check to opponent"].reset(new QSoundEffect);
	sounds["check to opponent"]->setSource(QUrl::fromLocalFile(":/sounds/check_to_opp"));
    sounds["check to opponent"]->setVolume(0.3f);
    sounds["invalid move"].reset(new QSoundEffect);
	sounds["invalid move"]->setSource(QUrl::fromLocalFile(":/sounds/invalid"));
    sounds["lose"].reset(new QSoundEffect);
	sounds["lose"]->setSource(QUrl::fromLocalFile(":/sounds/lose"));
    sounds["lose"]->setVolume(0.5f);
    sounds["win"].reset(new QSoundEffect);
	sounds["win"]->setSource(QUrl::fromLocalFile(":/sounds/win"));
	sounds["win"]->setVolume(0.8f);
    sounds["draw"].reset(new QSoundEffect);
	sounds["draw"]->setSource(QUrl::fromLocalFile(":/sounds/draw"));

	// settings init
	QSettings::setDefaultFormat(QSettings::IniFormat); // personal preference
	settings.setValue("user_name", "Lazy" +
		QString::number(std::rand() % (int)std::pow(10, max_nick - 4)));
	settings.setValue("opp_name", "Player2");
	settings.setValue("time_setup", 0);
	settings.setValue("match_side", false);
    settings.setValue("game_regime", "friend_offline");
    settings.setValue("ip_address", default_address);
    settings.setValue("port_address", default_port);

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
	pic_mask = pix.createMaskFromColor(Qt::transparent);

	// user and opponent avatars in game and settings
    ui->user_avatar->setMask(pic_mask);
    ui->opponent_avatar->setMask(pic_mask);
	ui->profile_avatar->setMask(pic_mask); // picture in the settings
	ui->profile_name->setText(settings.value("user_name").toString());
	ui->profile_avatar->setPixmap(user_pic);

	// friend_connect_tab > time limit buttons
	auto layout = ui->time_limits_layout;
	QPushButton* button;
	for (int i = 1; i < 10; i++) {
		button = qobject_cast<QPushButton*>(layout->itemAt(i)->widget());
        if (button)
            connect(button, &QPushButton::clicked, [this, button]() {
                int minutes_n = button->text().toInt(); //button->objectName().mid(3).toInt()
                settings.setValue("time_setup", minutes_n);
                });
	}

    // prepare scroll_area before making a chat
    rounded_area->setStyleSheet("QAbstractScrollArea{background: transparent; border: none;}");
    rounded_area->setGeometry(ui->chat_area->geometry());
    rounded_area->setSizePolicy(ui->chat_area->sizePolicy());
    rounded_area->setMinimumSize(ui->chat_area->minimumSize());
    rounded_area->setMaximumSize(ui->chat_area->maximumSize());
    rounded_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rounded_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->game_grid->replaceWidget(ui->chat_area, rounded_area.data());
    ui->chat_area->~QScrollArea();



	// chat
    max_message_width = rounded_area->minimumWidth() - 20;
    message_layout->setContentsMargins(10, 5, 10, 5);
    message_box->setLayout(message_layout.data());
    message_box->resize(rounded_area->width() - 28, 0);
	message_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    message_box->setStyleSheet("background-color: transparent;"); //#1B1C1F
    rounded_area->setWidget(message_box.data());
    rounded_area->setWidgetResizable(true);

    // match history
    history_area->setStyleSheet("QAbstractScrollArea{background: transparent; border: none;}");
    history_area->setGeometry(ui->match_history->geometry());
    history_area->setSizePolicy(ui->match_history->sizePolicy());
    history_area->setMinimumSize(ui->match_history->minimumSize());
    history_area->setMaximumSize(ui->match_history->maximumSize());
    history_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    history_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->game_grid->replaceWidget(ui->match_history, history_area.data());
    ui->match_history->~QLabel();

    history_label->setStyleSheet("background-color: transparent;");
    history_label->setFont({ "Segoe UI", 12 });
    history_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    history_area->setWidget(history_label.data());
    history_area->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    history_area->setWidgetResizable(true);
}

#include "mainwindow_buttons.h"

void MainWindow::openTab(QWidget* page)
{
    //QScopedPointer<QWidget> tab_ptr(ui->tabWidget->currentWidget());
    if (!ui || !ui->tabWidget || !page)
     {
        qDebug() << curTime() << "ERROR: in open tab with pointers";
        return;
     }
    last_tab = ui->tabWidget->currentWidget();
    if (!last_tab)
     {
        qDebug() << curTime() << "ERROR: in open tab with last_tab pointer";
        return;
     }
    ui->tabWidget->setCurrentWidget(page);
    auto scroller = rounded_area->horizontalScrollBar();
    scroller->setValue(scroller->maximum());
}

void MainWindow::openStopGameDialog()
{
    showBox("Stop active game",
            "Stop your current game at first.",
            QMessageBox::Warning);
}

void MainWindow::writeStory(int order, halfmove move)
{
    virtu vf = move.move.first;
    virtu vt = move.move.second;
    char piece = vf.name;
    Tile *from = vf.tile;
    Tile *to = vt.tile;
    char promo = move.promo;
    scoord f = from->coord;
    scoord t = to->coord;

    QString out;
    if (move.castling){
        out = "O-O";
    }
    else{
        if (piece != 'P')
            out += piece;
        out += coordToString(f) + coordToString(t);
        if (promo != 'e')
            out += "=" + QString(promo);
    }

    if (order % 2)
        history_label->setText(history_label->text() + QString::number((order-1) / 2 + 1) + ". " + out + " ");
    else
        history_label->setText(history_label->text() + out + " ");

    history_label->adjustSize();
//    QTimer::singleShot(100, [&]() {
        auto scroller = history_area->horizontalScrollBar();
        scroller->setValue(scroller->maximum());
//        });
}

QString MainWindow::coordToString(scoord coord)
{
    return QString(char('a' + coord.x)) + QString::number(coord.y + 1);
}

void MainWindow::showStatus(const QString& status) {
    ui->statusBar->showMessage(status, 0);
}

void MainWindow::switchGlow() // FIX: should be changed for different sides
{
    bool match_side = settings.value("match_side").toBool();
    if (board->turn == match_side)
        ui->user_avatar->setGraphicsEffect(avatar_effect.data());
    else
        ui->opponent_avatar->setGraphicsEffect(avatar_effect.data());
    this->update();
}

void MainWindow::startGame() // side true for user - white
{
    QString game_type = settings.value("game_regime").toString();
    if (game_active && game_type == "friend_online"){
        endSlot(endnum::interrupt);
    }
    if (game_type == "friend_online"){
        ui->actionProfile->setEnabled(false);
        ui->message_edit->setPlainText("Great move! Have you studied in a clown school?");
        ui->message_edit->installEventFilter(this);
    }
    else if (game_type == "friend_offline"){
        settings.setValue("match_side", true);
        opp_pic = default_pic;
        settings.setValue("opp_name", "Friend");
        ui->message_edit->setPlainText("Chat is off. But you can chat with yourself if you are a hikikomori.");
    }
	openTab(ui->game_tab);
    activateWindow();
	ui->user_avatar->setPixmap(user_pic);
	ui->user_name->setText(settings.value("user_name").toString());
	ui->opponent_avatar->setPixmap(opp_pic);
	ui->opponent_name->setText(settings.value("opp_name").toString());
	bool match_side = settings.value("match_side").toBool();
    (match_side ? ui->user_avatar : ui->opponent_avatar)->setGraphicsEffect(avatar_effect.data());
	for (QLayoutItem* child; (child = message_layout->takeAt(0)) != nullptr; child->widget()->~QWidget()) {}
    message_box->resize(rounded_area->width(), 0);
    ui->statusBar->show();
    history_label->clear();
    history_label->adjustSize();

    if (board)
        board.reset(new Board(board.data(), settings));
    else if (ui->board_background){
        board.reset(new Board(ui->board_background, settings));
        ui->board_background->~QLabel();
    }
    else
        qDebug() << curTime() << "ERROR: in MainWindow::startGame() with board/ui->background pointer";

    // old board will be destroyed inside Board constructor
    connect(board.data(), &Board::newStatus, this, &MainWindow::statusSlot);
    connect(board.data(), &Board::theEnd, this, &MainWindow::endSlot);

    if (game_type == "friend_online"){
        connect(board.data(), &Board::moveMade, [this](scoord from, scoord to, char promotion_type) {
            net->sendToServer(package_ty::move, {}, {}, from, to, promotion_type);
            });

        int time = settings.value("time_setup").toInt();
        clock.reset(new ChessClock(0/*board.data()*/, ui->opponent_timer, ui->user_timer, match_side, time));
        // old clock will be destroyed inside Board constructor as a child // FIX: at least it should be
        connect(this, &MainWindow::timeToSwitchTime, clock.data(), &ChessClock::switchTimer);
        connect(clock.data(), &ChessClock::userOut, [this]() {
            endSlot(endnum::user_out_of_time);
            });
        connect(clock.data(), &ChessClock::opponentOut, [this]() {
            endSlot(endnum::opponent_out_of_time);
            });

        connect(ui->resign_button, &QPushButton::clicked, this, &MainWindow::on_resign_button_clicked);
        connect(ui->draw_button, &QPushButton::clicked, this, &MainWindow::on_draw_button_clicked);
        connect(this, &MainWindow::editReturnPressed, this, &MainWindow::editReturnSlot);

        clock->startTimer();
    }
    else if (game_type == "friend_offline"){
        ui->user_timer->setText("");
        ui->opponent_timer->setText("");
    }

	showStatus("Ready? Go!");
    game_active = true;

}

void MainWindow::endSlot(endnum end_type)  // FIX: white_wins and black_wins enum values should
// be changed to user_wins and opp_wins, and what color user plays should be checked in Board::reactOnClick();
{
    if (!game_active){
        qDebug() << curTime() << "Application tried to close inactive game";
        return;
    }
    if (clock)
        clock->stopTimer();
	board->setEnabled(false);
	ui->draw_button->disconnect();
	ui->resign_button->disconnect();
	ui->actionProfile->setEnabled(true);
    disconnect(this, &MainWindow::editReturnPressed, this, &MainWindow::editReturnSlot);
    game_active = false;
    ui->statusBar->hide();
    ui->message_edit->removeEventFilter(this);

    QString opp_name = settings.value("opp_name").toString();
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

    if (settings.value("game_regime").toString() == "friend_online")
        net->sendToServer(package_ty::end_game);
    showStatus(info_message);
    QMessageBox msg_box;
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
//        qDebug() << curTime() << "Counted moves:" << board->valid->countMovesTest(i);
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
		showStatus(board->turn ? "White's turn" : "Black's turn");
		break;
    case tatus::opponent_piece_eaten:
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
		break;
	}
	switchGlow();
	emit timeToSwitchTime();
    size_t order = board->history.size();
    halfmove last_move = board->history.back();
    writeStory(order, last_move);
}

void MainWindow::printMessage(QString name, bool own, QString text)
{
	if (text.isEmpty())
		return;

	QChar ch;
	for (int cur_len = 0, pos = 0; pos < text.size(); pos++) {
		ch = text[pos];
		if (ch == ' ' || ch == '\n')
			cur_len = 0;
		else {
			cur_len += message_metrics.size(0, ch).width();
			if (cur_len > (max_message_width - 10)) {
				text.insert(pos, "\n");
				cur_len = 0;
			}
		}
	}

    QSharedPointer<QLabel> message(new QLabel(this));
    if (own)
        message->setStyleSheet("background-color: rgb(0,179,60); border-radius: 14;");
    else
        message->setStyleSheet("background-color: rgb(0,128,21); border-radius: 14;");
	message->setIndent(5);
	message->setMargin(5);
	message->setMaximumWidth(max_message_width);
	message->setFont(message_font);
	message->setWordWrap(true);
	message->setText(name + "\n" + text);
	message->adjustSize();
	message->setMinimumSize(message->size());
	message->setMaximumSize(message->size());
    if (own)
        message_layout->addWidget(message.data(), 0, Qt::AlignTop | Qt::AlignRight);
    else
        message_layout->addWidget(message.data(), 0, Qt::AlignTop | Qt::AlignLeft);
    message_box->resize(message_box->width(), message_box->height() + message->height() + 10);
	// 10 is layout margin here, for shortness

	QTimer::singleShot(100, [&]() {
        auto scroller = rounded_area->verticalScrollBar();
		scroller->setValue(scroller->maximum());
		});
}

void MainWindow::on_actionAbout_triggered()
{
    QString title = "About Mercury Chess";
    QString description = "I am Max Cury (MMD18) and this is my pet project written in C++ programming language with Qt framework.\n\n"
            "It may seem that the main feature of the program is a chess game with a beautiful style, but it's absolutely not.\n\nThe main feature is its built-in archiver "
            "with its own chess data format using a sophisticated algorithm to compress the data, so that it takes even less than 1 byte of space for every halfmove and uses its own "
            "bitstream allowing it to operate individual bits in std::cout style (available on github by https://github.com/redmms/finestream.git).\n\nI also invented several algorithms for describing chess board positions "
            "(like FEN but with a different structure and encoded in bits) and will code them if I find some "
            "interest of programmers or chess community. "
            "\n\n"
            "To contact me, use this email: mmd18cury@yandex.ru";
    QMessageBox::about(0, title, description);
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(0, "About Qt");
}

void MainWindow::on_actionSave_game_triggered()
{
//    Move decoder(220);
    

}

