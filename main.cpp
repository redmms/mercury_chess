//#define QT_MESSAGELOGCONTEXT 
#pragma once
#include <QApplication>
#include "app/mainwindow.h"
#include "app/debug_message_handler.hpp"
//#include <windows.h>  // Äëÿ AllocConsole()
//#include <cstdio>  
int main(int argc, char *argv[])
{
    LogHandler handler("log.txt");
    qInstallMessageHandler(&LogHandler::messageHandler);
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);
    //freopen("CONOUT$", "w", stderr);

    QApplication app(argc, argv);
    app.setOrganizationName("MMD18 soft");
    app.setApplicationName("MercuryChess" + curTime());
    QString app_dir = app.applicationDirPath();
    MainWindow mainwindow(app_dir);
    mainwindow.showMaximized();
    return app.exec();
}
