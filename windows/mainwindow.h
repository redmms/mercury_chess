#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "..\src\game\board.h"
#include "..\src\game\local_types.h"
#include <QGraphicsDropShadowEffect>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow* ui;
    Board* board;
    QGraphicsDropShadowEffect * avatar_effect = new QGraphicsDropShadowEffect;

    void showStatus(QString status);
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void statusSlot(setatus status);
    void endSlot(endnum end_type);
};

#endif // MAINWINDOW_H
