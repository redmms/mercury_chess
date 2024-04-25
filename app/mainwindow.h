#pragma once
#include "../app/local_types.h"
#include <QMainWindow>
#include <QPointer>
#include "ui_mainwindow.h"

namespace Ui
{
    class MainWindow;
}

class QGraphicsDropShadowEffect;
class QSoundEffect;
class QEvent;
namespace mmd 
{
    class Chat;
    class WebClient;
    class HistoryArea;
    class Board;
    class ChessClock;
    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        QScopedPointer<Ui::MainWindow> ui;
        QPointer<HistoryArea> history_area;
        QPointer<ChessClock> clock;
        QPointer<Board> board;
        QPointer<Chat> chat;
        QPointer<WebClient> net;
        std::map<soundnum, QPointer<QSoundEffect>> sounds;
        QPointer<QGraphicsDropShadowEffect> avatar_effect;
        QPointer<QWidget> last_tab;
        bool game_active;
        lognum login_regime;

        MainWindow();

        void showStatus(const QString& status);
        void switchGlow();
        bool eventFilter(QObject* object, QEvent* event);
        void openStopGameBox();
        int changeLocalName(QString name);
        void startGame(QString game_regime);
        void openTab(QWidget* page);

    signals:
        void timeToSwitchTime();
        void editReturnPressed();

    private slots:
        // user defined slots
        void statusSlot(estatus status);
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
        void on_copy_fen_button_clicked();

        // menu's actions
        void on_actionWith_friend_triggered();
        void on_actionWith_friend_offline_triggered();
        void on_actionTraining_triggered();
        void on_actionProfile_triggered();
        void on_actionToggle_fullscreen_triggered();
        void on_actionEnter_triggered();
        void on_actionAbout_triggered();
        void on_actionAbout_Qt_triggered();
        void on_actionSave_game_triggered();
        void on_actionLoad_game_triggered();
        void on_actionRules_triggered();
        void on_actionSend_suggestion_triggered();
        void on_actionReport_a_bug_triggered();

        // user defined button slots
        void my_offline_stop_button_clicked();
        void my_offline_back_button_clicked();
        void my_history_next_button_clicked();
        void my_history_previous_button_clicked();

        friend class WebClient;
    protected slots:
        void endSlot(endnum end_type);
    };
}
//void on_test_button_clicked();
//void on_actionWith_AI_triggered();
//void on_actionRandomly_triggered();
