#pragma once
#include <QApplication>
#include "windows\mainwindow.h"
#include "game\tile.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("MMD18 soft");
    app.setApplicationName("Mercury Chess");
    MainWindow *main_window = new MainWindow();
    main_window->showMaximized();
    return app.exec();
}

