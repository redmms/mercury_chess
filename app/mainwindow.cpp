#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "C:\Qt_projects\multicolor_chess\src\game\board.h"
#include "C:\Qt_projects\multicolor_chess\src\game\clock.h"
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

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	board(nullptr),
	clock(nullptr),
	net(new WebClient(this)),
	sounds{},
	settings{},
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
    rounded_area(new RoundedScrollArea(this)),
    game_active(false),
    registering(false)
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

	std::srand(std::time(nullptr));

	// sounds init
	sounds["move"] = new QSoundEffect;
	sounds["move"]->setSource(QUrl::fromLocalFile(":/sounds/move"));
	sounds["user's piece eaten"] = new QSoundEffect;
	sounds["user's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/user_eaten"));
	sounds["user's piece eaten"]->setVolume(0.6f);
	sounds["opponent's piece eaten"] = new QSoundEffect;
	sounds["opponent's piece eaten"]->setSource(QUrl::fromLocalFile(":/sounds/opp_eaten"));
	sounds["opponent's piece eaten"]->setVolume(0.7f);
	sounds["castling"] = new QSoundEffect;
	sounds["castling"]->setSource(QUrl::fromLocalFile(":/sounds/castling"));
	sounds["promotion"] = new QSoundEffect;
	sounds["promotion"]->setSource(QUrl::fromLocalFile(":/sounds/promotion"));
	sounds["check to user"] = new QSoundEffect;
	sounds["check to user"]->setSource(QUrl::fromLocalFile(":/sounds/check"));
	sounds["check to user"]->setVolume(0.7f);
	sounds["check to opponent"] = new QSoundEffect;
	sounds["check to opponent"]->setSource(QUrl::fromLocalFile(":/sounds/check_to_opp"));
    sounds["check to opponent"]->setVolume(0.5f);
	sounds["invalid move"] = new QSoundEffect;
	sounds["invalid move"]->setSource(QUrl::fromLocalFile(":/sounds/invalid"));
	sounds["lose"] = new QSoundEffect;
	sounds["lose"]->setSource(QUrl::fromLocalFile(":/sounds/lose"));
    sounds["lose"]->setVolume(0.5f);
	sounds["win"] = new QSoundEffect;
	sounds["win"]->setSource(QUrl::fromLocalFile(":/sounds/win"));
	sounds["win"]->setVolume(0.8f);
	sounds["draw"] = new QSoundEffect;
	sounds["draw"]->setSource(QUrl::fromLocalFile(":/sounds/draw"));

	// settings init
	QSettings::setDefaultFormat(QSettings::IniFormat); // personal preference
	settings.setValue("user_name", "Lazy" +
		QString::number(std::rand() % (int)std::pow(10, max_nick - 4)));
	settings.setValue("opp_name", "Player2");
	settings.setValue("time_setup", 0);
	settings.setValue("match_side", false);

	// glow effect for avatars
    avatar_effect->setBlurRadius(40);
	avatar_effect->setOffset(0, 0);
	avatar_effect->setColor(Qt::green);

	// mask for rounded borders on avatars
	auto mask_size = ui->user_avatar->width();
	QPixmap  pix(mask_size, mask_size); // initialize a mask for avatar's rounded corners
	pix.fill(Qt::transparent);
	QPainter painter(&pix);
	painter.setBrush(Qt::color1);
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
    ui->game_grid->replaceWidget(ui->chat_area, rounded_area);
    ui->chat_area->~QScrollArea();

	// chat
    max_message_width = rounded_area->minimumWidth() - 20;
    message_layout->setContentsMargins(10, 5, 10, 5);
	message_box->setLayout(message_layout);
    message_box->resize(rounded_area->width() - 28, 0);
	message_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    message_box->setStyleSheet("background-color: transparent;"); //#1B1C1F
    rounded_area->setWidget(message_box);
    rounded_area->setWidgetResizable(true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

#include "mainwindow_buttons.h"

void MainWindow::openTab(QWidget* page)
{
    last_tab = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(page);
}

void MainWindow::openStopGameDialog()
{
    QMessageBox msg_box;
    msg_box.setWindowTitle("Stop active game");
    msg_box.setText("Stop your current game at first.");
    msg_box.setIcon(QMessageBox::Warning);
    msg_box.addButton("Resign", QMessageBox::AcceptRole);
    msg_box.addButton("Back to the board", QMessageBox::RejectRole);
    connect(&msg_box, &QMessageBox::accepted, this, &MainWindow::on_resign_button_clicked);
    msg_box.exec();
}

void MainWindow::showStatus(const QString& status) {
    ui->statusBar->showMessage(status, 0);
}

void MainWindow::switchGlow() // FIX: should be changed for different sides
{
    bool match_side = settings.value("match_side").toBool();
    if (board->turn == match_side)
        ui->user_avatar->setGraphicsEffect(avatar_effect);
    else
        ui->opponent_avatar->setGraphicsEffect(avatar_effect);
    this->update();
}

void MainWindow::startGame() // side true for user - white
{
	openTab(ui->game_tab);
    activateWindow();
	ui->user_avatar->setPixmap(user_pic);
	ui->user_name->setText(settings.value("user_name").toString());
	ui->opponent_avatar->setPixmap(opp_pic);
	ui->opponent_name->setText(settings.value("opp_name").toString());
	bool match_side = settings.value("match_side").toBool();
	(match_side ? ui->user_avatar : ui->opponent_avatar)->setGraphicsEffect(avatar_effect);
	for (QLayoutItem* child; (child = message_layout->takeAt(0)) != nullptr; child->widget()->~QWidget()) {}
    message_box->resize(rounded_area->width(), 0);
	ui->message_edit->setPlainText("Great move! Have you studied in a clown school?");
	ui->actionProfile->setEnabled(false);
    ui->statusBar->show();
    ui->message_edit->installEventFilter(this);

	board = new Board(board != nullptr ? board : ui->board_background, match_side);
	// old board will be destroyed inside Board constructor
	connect(board, &Board::newStatus, this, &MainWindow::statusSlot);
	connect(board, &Board::theEnd, this, &MainWindow::endSlot);
    connect(board, &Board::moveMade, [this](scoord from, scoord to, char promotion_type) {
        net->sendToServer(package_ty::move, {}, {}, from, to, promotion_type);
		});

	int time = settings.value("time_setup").toInt();
	clock = new ChessClock(board, ui->opponent_timer, ui->user_timer, match_side, time);
	// old clock will be destroyed inside Board constructor as a child // FIX: at least it should be
	connect(this, &MainWindow::timeToSwitchTime, clock, &ChessClock::switchTimer);
	connect(clock, &ChessClock::userOut, [this]() {
		endSlot(endnum::user_out_of_time);
		});
	connect(clock, &ChessClock::opponentOut, [this]() {
		endSlot(endnum::opponent_out_of_time);
		});

	connect(ui->resign_button, &QPushButton::clicked, this, &MainWindow::on_resign_button_clicked);
	connect(ui->draw_button, &QPushButton::clicked, this, &MainWindow::on_draw_button_clicked);
    connect(this, &MainWindow::editReturnPressed, this, &MainWindow::editReturnSlot);

	showStatus("Ready? Go!");
	clock->startTimer();
    game_active = true;
}

void MainWindow::endSlot(endnum end_type)  // FIX: white_wins and black_wins enum values should
// be changed to user_wins and opp_wins, and what color user plays should be checked in Board::reactOnClick();
{
    if (!game_active){
        qDebug() << "Application tried to close inactive game";
        return;
    }
	QString opp_name = settings.value("opp_name").toString();
	QString info_message;
    auto icon_type = QMessageBox::Information;
	switch (end_type) {
	case endnum::draw_by_agreement:
		sounds["draw"]->play();
		info_message = "Draw by agreement";
		break;
	case endnum::draw_by_stalemate:
		sounds["draw"]->play();
		info_message = "Draw by stalemate";
        net->sendToServer(package_ty::end_game);
		break;
	case endnum::user_wins:
		sounds["win"]->play();
		info_message = "You win by checkmate";
        net->sendToServer(package_ty::end_game);
		break;
	case endnum::opponent_wins:
		sounds["lose"]->play();
		info_message = opp_name + " wins by checkmate";
        net->sendToServer(package_ty::end_game);
		break;
	case endnum::user_resignation:
		sounds["lose"]->play();
		info_message = opp_name + " wins by your resignation";
		break;
	case endnum::opponent_resignation:
		sounds["win"]->play();
		info_message = "You win by " + opp_name + "'s resignation";
        net->sendToServer(package_ty::end_game);
		break;
	case endnum::user_out_of_time:
		sounds["lose"]->play();
		info_message = opp_name + " wins by your timeout";
        net->sendToServer(package_ty::end_game);
		break;
	case endnum::opponent_out_of_time:
		sounds["win"]->play();
		info_message = "You win by " + opp_name + "'s timeout";
        net->sendToServer(package_ty::end_game);
        break;
    case endnum::opponent_disconnected_end:
        icon_type = QMessageBox::Critical;
        info_message = "Opponent disconnected. Maybe he doesn't like to play with you?";
        break;
    case endnum::server_disconnected:
        icon_type = QMessageBox::Critical;
        info_message = "Lost connection with server";
        break;
	}
	showStatus(info_message);

	clock->stopTimer();
	board->setEnabled(false);
	ui->draw_button->disconnect();
	ui->resign_button->disconnect();
	ui->actionProfile->setEnabled(true);
    disconnect(this, &MainWindow::editReturnPressed, this, &MainWindow::editReturnSlot);
    game_active = false;
    ui->statusBar->hide();
    ui->message_edit->removeEventFilter(this);

    if (end_type != endnum::server_disconnected){
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
}

void MainWindow::statusSlot(tatus status)
{
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

	QLabel* message = new QLabel(this);
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
        message_layout->addWidget(message, 0, Qt::AlignTop | Qt::AlignRight);
    else
        message_layout->addWidget(message, 0, Qt::AlignTop | Qt::AlignLeft);
    message_box->resize(message_box->width(), message_box->height() + message->height() + 10);
	// 10 is layout margin here, for shortness

	QTimer::singleShot(100, [&]() {
        auto scroller = rounded_area->verticalScrollBar();
		scroller->setValue(scroller->maximum());
		});
}
