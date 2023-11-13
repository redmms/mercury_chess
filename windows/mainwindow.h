#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "enums.h"
enum endnum : int;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow* ui;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeStatus(QString status);
    void endSlot(endnum end_type);
};

#endif // MAINWINDOW_H
