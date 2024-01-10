#ifndef DEBUG_MESSAGE_HANDLER_H
#define DEBUG_MESSAGE_HANDLER_H

#include <QtGlobal>
#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString log_file_name = "log.txt";
    QFile log_file(log_file_name);
    QByteArray localMsg = msg.toLocal8Bit();
    if (log_file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream text_stream(&log_file);
        switch (type) {
        case QtDebugMsg:
            text_stream << "Debug in:\n";
            break;
        case QtInfoMsg:
            text_stream << "Info in:\n";
            break;
        case QtWarningMsg:
            text_stream << "Warning in:\n";
            break;
        case QtCriticalMsg:
            text_stream << "Critical in:\n";
            break;
        case QtFatalMsg:
            text_stream << "Fatal in:\n";
            abort();
            break;
        default:
            text_stream << "Custom message in:\n";
            break;
        }
        text_stream << context.file
                    << ", "
                    << context.line
                    << ", "
                    << context.function
                    << "\n"
                    << localMsg.constData()
                    << "\n\n";
    }
}

#endif // DEBUG_MESSAGE_HANDLER_H
