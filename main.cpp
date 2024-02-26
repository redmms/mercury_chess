//#define QT_MESSAGELOGCONTEXT 
#include <QApplication>
#include "app/mainwindow.h"
#include "app/debug_message_handler.hpp"



int main(int argc, char *argv[])
{
    LogHandler handler("log.txt");
    qInstallMessageHandler(&LogHandler::messageHandler);
    QApplication* app = new QApplication(argc, argv);
    app->setOrganizationName("MMD18 soft");
    app->setApplicationName("Mercury Chess");
    QString app_dir = app->applicationDirPath();
    MainWindow *mainwindow = new MainWindow(0, app_dir, app);
    mainwindow->showMaximized();
    return app->exec();
}
