//#define QT_MESSAGELOGCONTEXT 
#pragma once
#include <QApplication>
#include "app/mainwindow.h"
#include "app/debug_message_handler.hpp"

int main(int argc, char *argv[])
{
    LogHandler handler("log.txt");
    qInstallMessageHandler(&LogHandler::messageHandler);
    QApplication app(argc, argv);
    app.setOrganizationName("MMD18 soft");
    app.setApplicationName("MercuryChess" + curTime());
    QString app_dir = app.applicationDirPath();
    MainWindow mainwindow(0, app_dir, &app);
    mainwindow.showMaximized();
    return app.exec();
}
