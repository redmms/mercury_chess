#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qmainwindow.h"
#include "qbitmap.h"
#include "qfont.h"
#include "qfontmetrics.h"
#include "qpixmap.h"
#include "qsettings.h"
#include "qstring.h"
#include "rounded_scrollarea.h"
#include "webclient.h"
#include "qobject.h"
#include <bitset>
#include <map>
class Board;
class ChessClock;
class QEvent;
class QGraphicsDropShadowEffect;
class QObject;
class QSoundEffect;
class QVBoxLayout;
class QWidget;
class WebClient;

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	Board* board;
	ChessClock* clock;
	WebClient* net;
	std::map<std::string, QSoundEffect*> sounds;
	QGraphicsDropShadowEffect* avatar_effect;
	QBitmap pic_mask;
	QWidget* last_tab;
	const int max_nick;
	QVBoxLayout* message_layout;
	QWidget* message_box;
	QFont message_font;
	QFontMetrics message_metrics;
	int max_message_width;
    RoundedScrollArea* rounded_area;
    bool game_active;
    bool waiting_for_invite_respond;

	void showStatus(const QString& status);  // FIX: will const& cause problems or not?
	void switchGlow();
	bool eventFilter(QObject* object, QEvent* event);
    void openStopGameDialog();

	friend class WebClient;
protected:
    Ui::MainWindow* ui;
	QPixmap user_pic;
	QPixmap opp_pic;
    QPixmap default_pic;
    int login_regime;

	void startGame();
	void printMessage(QString name, bool own, QString text);
    void openTab(QWidget* page);

public:
	MainWindow(QWidget* parent = 0);
	~MainWindow();

    QSettings settings;

signals:
	void timeToSwitchTime();
    void editReturnPressed();

private slots:
	void statusSlot(tatus status);
    void editReturnSlot();

	void on_draw_button_clicked();
	void on_resign_button_clicked();
	void on_actionProfile_triggered();
	void on_change_photo_button_clicked();
	void on_change_name_button_clicked();
	void on_back_from_settings_clicked();
	void on_actionWith_friend_triggered();
	void on_registrate_button_clicked();
	void on_guest_button_clicked();
	void on_send_invite_button_clicked();
    void on_login_button_clicked();
    void on_end_login_button_clicked();
    void on_back_from_login_button_clicked();
    void on_actionToggle_fullscreen_triggered();

    void on_actionWith_friend_offline_triggered();

    void on_actionEnter_triggered();

protected slots:
	void endSlot(endnum end_type);
};

#endif // MAINWINDOW_H
