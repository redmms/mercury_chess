#include <QApplication>
#include "app\mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("MMD18 soft");
    app.setApplicationName("Mercury Chess 1");
    MainWindow *main_window = new MainWindow();
    main_window->showMaximized();
    return app.exec();
}

