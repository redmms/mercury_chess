#pragma once
#include "../app/local_types.h"
#include <fstream>
#include <streambuf>

namespace mmd 
{
    class LogHandler 
    {
        QString log_file_name;
        std::basic_streambuf<char>* coutbuf;
        std::basic_streambuf<char>* cerrbuf;
        std::ofstream old_ofstream;

    public:
        static std::ofstream log_ofstream;

        LogHandler(QString log_file_name_);
        ~LogHandler();
        static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    };
}
