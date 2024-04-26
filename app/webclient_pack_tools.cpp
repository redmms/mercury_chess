#include "webclient.h"
#include <QDebug>
#include <QBuffer>
using namespace std;

namespace mmd
{
    void WebClient::writePack(packnum data) {
        send_stream << quint8(data);
    }

    void WebClient::writePack(quint8 data) {
        send_stream << data;
    }

    void WebClient::writePack(bool data) {
        send_stream << data;
    }

    void WebClient::writePack(const QByteArray& data) {
        send_stream.writeBytes(data.data(), data.size());
    }

    void WebClient::writePack(const QString& data) {
        auto utf8_str = data.toUtf8();
        auto str_size = utf8_str.size();
        send_stream.writeBytes(utf8_str, str_size);
    }

    void WebClient::writePack(const QPixmap& data) {
        QByteArray arr;
        QBuffer buffer(&arr);
        buffer.open(QIODevice::WriteOnly);
        data.save(&buffer, "PNG");
        writePack(arr);
    }

    void WebClient::readPack(packnum& data)
    {
        quint8 byte;
        read_stream >> byte;
        if (byte < packnum::registration || byte > packnum::already_registered)
            qDebug() << "Error: packnum from read_package is out of range";
        else
            data = packnum(byte);
    }

    void WebClient::readPack(QByteArray& data) {
        uint len;
        read_stream >> len;
        QByteArray buffer(len, Qt::Uninitialized);
        read_stream.readRawData(buffer.data(), len);
        data = QByteArray(buffer);
    }

    void WebClient::readPack(QString& data) {
        uint len;
        char* buffer;
        read_stream.readBytes(buffer, len);
        data = QString(buffer);
    }

    void WebClient::readPack(QPixmap& data)
    {
        QByteArray arr;
        readPack(arr);
        if (!data.loadFromData(arr, "PNG"))
            qDebug() << "Error: couldn't load QPixmap from QByteArray in read_package";
    }

    void WebClient::readPack(bool& data) {
        read_stream >> data;
    }

    void WebClient::readPack(quint8& data) {
        read_stream >> data;
    }
}