#ifndef DEBUG_MESSAGE_HANDLER_H
#define DEBUG_MESSAGE_HANDLER_H

#include <QtGlobal>
#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <stdexcept>
#include "../app/local_types.hpp"
#include <QMessageLogContext>

std::ofstream log_ofstream;

class LogHandler {
public:
    QString log_file_name;
    std::basic_streambuf<char>* coutbuf;
    std::basic_streambuf<char>* cerrbuf;
    LogHandler(QString log_file_name_) : 
            log_file_name(log_file_name_)
    {
        
        log_ofstream = std::ofstream(log_file_name.toStdString(), std::ios::app);
        if (log_ofstream.is_open()) {
            coutbuf = std::cout.rdbuf(log_ofstream.rdbuf());
            cerrbuf = std::cerr.rdbuf(log_ofstream.rdbuf());
        }
        else {
            qWarning() << "\n" << curTime() << ": Couldn't open log file\n";
        }
    }
    ~LogHandler() {
        log_ofstream.close();
        std::cout.rdbuf(coutbuf);
        std::cerr.rdbuf(cerrbuf);
    }
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        if (log_ofstream.is_open()) {
            log_ofstream << "\n\n";
            switch (type) {
            case QtDebugMsg:
                log_ofstream << std::string("Debug ");
                break;
            case QtInfoMsg:
                log_ofstream << std::string("Info ");
                break;
            case QtWarningMsg:
                log_ofstream << std::string("Warning ");
                break;
            case QtCriticalMsg:
                log_ofstream << std::string("Critical ");
                break;
            case QtFatalMsg:
                log_ofstream << std::string("Fatal ");
                break;
            default:
                log_ofstream << std::string("Custom message ");
                break;
            }
            log_ofstream 
                << "\n"
                //<< context.file
                //<< ", "
                //<< context.line
                //<< ", "
                //<< context.function
                //<< "\n"
                << (curTime() + QString(": ")).toStdString()
                << msg.toStdString()
                << "\n\n";
            if (type == QtFatalMsg) {
                log_ofstream.close();
                abort();
            }
        }
    }
};


#endif // DEBUG_MESSAGE_HANDLER_H
