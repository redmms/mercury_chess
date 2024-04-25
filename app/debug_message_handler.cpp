#include "debug_message_handler.h"
#include <iostream>
#ifndef NDEBUG
#include <QMessageLogContext>
#endif
using namespace std;

namespace mmd 
{
    std::ofstream LogHandler::log_ofstream;

    LogHandler::LogHandler(QString log_file_name_) :
        log_file_name(log_file_name_),
        coutbuf(0),
        cerrbuf(0),
        old_ofstream{}
    {
        try {
            //if (log_ofstream) {
            old_ofstream = move(log_ofstream);
            //}
            log_ofstream = ofstream(log_file_name.toStdString(), ios::out | ios::app);
            
        }
        catch (const exception& err) {
            throw;
        }
        if (log_ofstream.is_open()) {
            coutbuf = cout.rdbuf(log_ofstream.rdbuf());
            cerrbuf = cerr.rdbuf(log_ofstream.rdbuf());
            log_ofstream << "\n" << curTime().toStdString() << " LOGGING SESSION STARTED\n";
        }
        else {
            cerr << "\n" << curTime().toStdString() << ": Couldn't open log file\n";
        }
    }

    LogHandler::~LogHandler() {
        log_ofstream.close();
        log_ofstream = move(old_ofstream);
        cout.rdbuf(coutbuf);
        cerr.rdbuf(cerrbuf);
    }

    void LogHandler::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        cerr << "\n\n";
        switch (type) {
        case QtDebugMsg:
            cerr << "Debug ";
            break;
        case QtInfoMsg:
            cerr << "Info ";
            break;
        case QtWarningMsg:
            cerr << "Warning ";
            break;
        case QtCriticalMsg:
            cerr << "Critical ";
            break;
        case QtFatalMsg:
            cerr << "Fatal ";
            break;
        default:
            cerr << "Custom message ";
            break;
        }
        cerr << "\n";
#ifndef NDEBUG
        cerr << context.file
             << ", "
             << context.line
             << ", "
             << context.function
             << "\n";
#endif           
        cerr << (curTime() + ": ").toStdString()
             << msg.toStdString()
             << "\n\n";
        if (type == QtFatalMsg) {
            log_ofstream.close();
        }
    }
}