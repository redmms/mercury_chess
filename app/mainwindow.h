#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qbitmap.h"
#include "qfont.h"
#include "qfontmetrics.h"
#include "qpixmap.h"
#include "qsettings.h"
#include "qstring.h"
#include "rounded_scrollarea.hpp"
#include "rounded_scrollarea_horizontal.hpp"
#include "webclient.h"
#include "qobject.h"
#include <bitset>
#include <map>
#include <QScopedPointer>
#include "../game/board.h"
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include "../game/clock.h"
#include <QVBoxLayout>
#include <QMainWindow>
#include <QWidget>
#include "ui_mainwindow.h"
#include <QSharedPointer>
#include <QPointer>
#include "../app/local_types.hpp"
#include <vector>
//import bitchess;
import simplechess;

class QEvent;
class QObject;
class QVBoxLayout;
class WebClient;

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
friend class WebClient;
friend class Board;
public:
    QScopedPointer<ChessClock> clock;
    QScopedPointer<WebClient> net;
    std::map<std::string, QScopedPointer<QSoundEffect>> sounds;
    QScopedPointer<QGraphicsDropShadowEffect> avatar_effect;
	QBitmap pic_mask;
    QPointer<QWidget> last_tab;
	const int max_nick;
    QScopedPointer<QVBoxLayout> message_layout;
    QScopedPointer<QWidget> message_box;
	QFont message_font;
	QFontMetrics message_metrics;
	int max_message_width;
    QScopedPointer<RoundedScrollArea> rounded_area;
    bool game_active;
    bool waiting_for_invite_respond;
    QString default_address;
    int default_port;
    QScopedPointer<HorizontalScrollArea> history_area;
    QScopedPointer<QLabel> history_label;
    QString app_dir;
    int current_move;
    QSettings settings;

	void showStatus(const QString& status);  // FIX: will const& cause problems or not?
	void switchGlow();
	bool eventFilter(QObject* object, QEvent* event);
    void openStopGameDialog();
    void openInDevDialog();
    void writeStory(int order, halfmove move);
    QString coordToString(scoord coord);
    int changeLocalName(QString name);
    //int changeOnlineName(QString name);



	MainWindow(QWidget* parent = 0, QString app_dir_ = "", QApplication* app = 0);
    ~MainWindow(){
        delete ui;
    }

    QPointer<QApplication> app;
    QScopedPointer<Board> board;
    Ui::MainWindow* ui;
    QPixmap user_pic;
    QPixmap opp_pic;
    QPixmap default_pic;
    int login_regime;

    void startGame(QString game_regime);
    void printMessage(QString name, bool own, QString text);
    void openTab(QWidget* page);

signals:
	void timeToSwitchTime();
    void editReturnPressed();

private slots:
    // user defined slots
	void statusSlot(tatus status);
    void editReturnSlot();

    // buttons from .ui file
	void on_draw_button_clicked();
	void on_resign_button_clicked();
	void on_change_photo_button_clicked();
	void on_change_name_button_clicked();
	void on_back_from_settings_clicked();
	void on_registrate_button_clicked();
	void on_guest_button_clicked();
	void on_send_invite_button_clicked();
    void on_login_button_clicked();
    void on_end_login_button_clicked();
    void on_back_from_login_button_clicked();
    void on_change_ip_button_clicked();
    void on_restore_default_button_clicked();

    // menu's actions
    void on_actionWith_friend_triggered();
    void on_actionWith_friend_offline_triggered();
    void on_actionProfile_triggered();
    void on_actionToggle_fullscreen_triggered();
    void on_actionEnter_triggered();
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionSave_game_triggered();
    void on_actionLoad_game_triggered();

    // menu's actions in development
    void on_actionWith_friend_2_triggered();
    void on_actionTraining_triggered();
    void on_actionRandomly_triggered();
    void on_actionRules_triggered();
    void on_actionSend_suggestion_triggered();
    void on_actionReport_a_bug_triggered();

    // user defined button slots
    void on_offline_stop_button_clicked();
    void on_offline_back_button_clicked();
    void on_history_next_button_clicked();
    void on_history_previous_button_clicked();

protected slots:
	void endSlot(endnum end_type);
};

#endif // MAINWINDOW_H
