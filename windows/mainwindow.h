#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
};

#endif // MAINWINDOW_H
