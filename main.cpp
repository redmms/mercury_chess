#include <QApplication>
#include "app/mainwindow.h"
#include "app/debug_message_handler.h"

int main(int argc, char *argv[])
{
    //qInstallMessageHandler(messageHandler);
    QApplication app(argc, argv);
    app.setOrganizationName("MMD18 soft");
    app.setApplicationName("Mercury Chess");
    MainWindow *mainwindow = new MainWindow();
    mainwindow->showMaximized();
    return app.exec();
}

