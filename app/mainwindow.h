#pragma once
#include "../app/local_types.h"
#include <QMainWindow>
#include <QBitmap>
#include <QPointer>

namespace Ui
{
    class MainWindow;
}

class WebClient;
class RoundedScrollArea;
class HorizontalScrollArea;
class Board;
class ChessClock;
class OfflineDialog;
class QPixmap;
class QFont;
class QFontMetrics;
class QGraphicsDropShadowEffect;
class QSoundEffect;
class QEvent;
class QVBoxLayout;
class MainWindow : public QMainWindow
{
    Q_OBJECT
friend class WebClient;
friend class Board;
public:
    Ui::MainWindow* ui;
    QString app_dir;
    QPointer<Board> board;
    QPointer<ChessClock> clock;
    QPointer<WebClient> net;
    QPointer<RoundedScrollArea> rounded_area;
    QPointer<HorizontalScrollArea> history_area;
    QPointer<QLabel> history_label;
    std::map<std::string, QPointer<QSoundEffect>> sounds;
    QPointer<QGraphicsDropShadowEffect> avatar_effect;
    QPointer<QWidget> last_tab;

    QPointer<QVBoxLayout> message_layout;
    QPointer<QWidget> message_box;
    QFont message_font;
    QFontMetrics message_metrics;
    int max_message_width;

    bool game_active;
    int login_regime;

    void showStatus(const QString& status);  // FIX: will const& cause problems or not?
    void switchGlow();
    bool eventFilter(QObject* object, QEvent* event);
    void openStopGameBox();
    void openInDevBox();
    void writeStory(int order, halfmove hmove);
    static QString halfmoveToString(halfmove hmove);
    static QString coordToString(scoord coord);
    int changeLocalName(QString name);
    //int changeOnlineName(QString name);

    MainWindow(QString app_dir_);
    ~MainWindow();

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
    void on_actionWith_AI_triggered();
    void on_actionTraining_triggered();
    void on_actionRandomly_triggered();
    void on_actionRules_triggered();
    void on_actionSend_suggestion_triggered();
    void on_actionReport_a_bug_triggered();

    // user defined button slots
    void my_offline_stop_button_clicked();
    void my_offline_back_button_clicked();
    void my_history_next_button_clicked();
    void my_history_previous_button_clicked();

    void on_test_button_clicked();

protected slots:
    void endSlot(endnum end_type);
};