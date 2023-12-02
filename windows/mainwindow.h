#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "..\src\game\board.h"
#include "..\src\game\local_types.h"
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include <map>
#include <QSettings>
#include <QPixmap>
#include <QBitmap>
#include <QTimer>
#include "..\src\game\clock.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow* ui;
    Board* board = nullptr;
    ChessClock* clock = nullptr;
    std::map<std::string, QSoundEffect*> sounds;
    QSettings settings;
    QGraphicsDropShadowEffect * avatar_effect = new QGraphicsDropShadowEffect;
    QPixmap user_pic = QPixmap(":images/profile");
    QPixmap opp_pic = QPixmap(":images/profile");
    QBitmap pic_map;
    QWidget* last_tab;
    bool match_side = true;

    void showStatus(const QString& status);  // FIX: will const& cause problems or not?
    void switchGlow();
    void startGame(bool side, int time);  // parameters: bool side - shows which color will user have,
    //int time (in milliseconds)

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void timeToSwitchTime();

private slots:
    void statusSlot(tatus status);
    void endSlot(endnum end_type);
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
};

#endif // MAINWINDOW_H
