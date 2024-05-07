#include "app/mainwindow.h"
#include "app/debug_message_handler.h"
#include <QDebug>
#ifdef MMDTEST
#include <windows.h>  // For AllocConsole()          
#include <cstdio>
#endif  

int main(int argc, char *argv[])
{
    mmd::LogHandler handler("log.txt");
    qInstallMessageHandler(&mmd::LogHandler::messageHandler);
#ifdef MMDTEST
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    QApplication app(argc, argv);
    app.setOrganizationName("MMD18 soft");
    app.setApplicationName("MercuryChess" + mmd::curTime());
    mmd::MainWindow mainwindow;
    mainwindow.showMinimized();
    return app.exec();
}
