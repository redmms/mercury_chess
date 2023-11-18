#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "..\src\game\board.h"
#include "..\src\game\local_types.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow* ui;

    void showStatus(QString status);
    Board* board;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void statusSlot(setatus status);
    void endSlot(endnum end_type);
};

#endif // MAINWINDOW_H
