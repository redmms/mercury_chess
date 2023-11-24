#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "..\src\game\board.h"
#include "..\src\game\local_types.h"
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include <map>

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
};

#endif // MAINWINDOW_H
