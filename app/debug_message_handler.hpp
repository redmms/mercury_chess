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

static std::ofstream* text_stream;

class LogHandler {
public:
    QString log_file_name;
    std::basic_streambuf<char>* coutbuf;
    std::basic_streambuf<char>* cerrbuf;
    LogHandler(QString log_file_name_) : 
            log_file_name(log_file_name_)
    {

        text_stream = new std::ofstream(log_file_name.toStdString(), std::ios::app);
        if (text_stream->is_open()) {
            coutbuf = std::cout.rdbuf(text_stream->rdbuf());
            cerrbuf = std::cerr.rdbuf(text_stream->rdbuf());
        }
        else {
            qWarning() << "\n" << curTime() << ": Couldn't open log file\n";
        }
    }
    ~LogHandler() {
        text_stream->close();
        std::cout.rdbuf(coutbuf);
        std::cerr.rdbuf(cerrbuf);
        delete text_stream;
    }
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        //static QFile log_file(log_file_name);
        //QByteArray localMsg = msg.toLocal8Bit();
        if (text_stream->is_open()) {
            *text_stream << "\n\n";
            switch (type) {
            case QtDebugMsg:
                *text_stream << "Debug in:";
                break;
            case QtInfoMsg:
                *text_stream << "Info in:";
                break;
            case QtWarningMsg:
                *text_stream << "Warning in:";
                break;
            case QtCriticalMsg:
                *text_stream << "Critical in:";
                break;
            case QtFatalMsg:
                *text_stream << "Fatal in:";
                break;
            default:
                *text_stream << "Custom message in:";
                break;
            }
            *text_stream 
                << "\n"
                << context.file
                << ", "
                << context.line
                << ", "
                << context.function
                << "\n"
                << (curTime() + ": ").toStdString()
                << msg.toStdString()
                << "\n\n";
            if (type == QtFatalMsg) {
                text_stream->close();
                delete text_stream;
                abort();
            }
        }
    }
};


#endif // DEBUG_MESSAGE_HANDLER_H
