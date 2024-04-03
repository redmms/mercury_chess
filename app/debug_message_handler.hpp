#pragma once
#include "../app/local_types.h"
#include <QMessageLogContext>
#include <fstream>
#include <iostream>

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
            log_ofstream << "\n" << curTime().toStdString() << " LOGGING SESSION STARTED\n";
        }
        else {
            std::cerr << "\n" << curTime().toStdString() << ": Couldn't open log file\n";
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
                log_ofstream << "Debug ";
                break;
            case QtInfoMsg:
                log_ofstream << "Info ";
                break;
            case QtWarningMsg:
                log_ofstream << "Warning ";
                break;
            case QtCriticalMsg:
                log_ofstream << "Critical ";
                break;
            case QtFatalMsg:
                log_ofstream << "Fatal ";
                break;
            default:
                log_ofstream << "Custom message ";
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
                << (curTime() + ": ").toStdString()
                << msg.toStdString()
                << "\n\n";
            if (type == QtFatalMsg) {
                log_ofstream.close();
                abort();
            }
        }
    }
};