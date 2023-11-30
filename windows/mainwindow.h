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

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow* ui;
    Board* board;
    QGraphicsDropShadowEffect * avatar_effect = new QGraphicsDropShadowEffect;
    std::map<std::string, QSoundEffect*> sounds;
    QSettings* settings = new QSettings;
    QPixmap user_pic = QPixmap(":images/profile");
    QPixmap opp_pic = QPixmap(":images/profile");
    QBitmap pic_map;

    void showStatus(const QString& status);  // FIX: will const& cause problems or not?
    void switchGlow();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
};

#endif // MAINWINDOW_H
