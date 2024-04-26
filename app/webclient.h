#pragma once
#include "../app/local_types.h"

class QTcpSocket;
namespace mmd
{
    class MainWindow;
    class WebClient : public QObject
    {
        Q_OBJECT

    public:
        MainWindow* mainwindow;
        QTcpSocket* socket;
        QByteArray read_package;
        QByteArray send_package;
        QDataStream read_stream;
        QDataStream send_stream;

        WebClient(MainWindow* parent = nullptr);
        ~WebClient();

        void writePack(packnum);
        void writePack(quint8);
        void writePack(bool);
        void writePack(const QByteArray&);
        void writePack(const QString&);
        void writePack(const QPixmap&);

        void readPack(packnum&);
        void readPack(quint8&);
        void readPack(bool&);
        void readPack(QByteArray&);
        void readPack(QString&);
        void readPack(QPixmap&);

        void initSocket();
        bool checkConnection(packnum type = packnum::login);
        bool connectToServer();
        void packFromSock(QTcpSocket* socket, QByteArray& received_package);
        void connectNewHost();

    signals:
        void endedReadingInvite();

    public slots:
        void sendToServer(packnum type, bool respond = false, QString message = "", scoord from = {}, scoord to = {}, char promotion_type = 'e');
        void readFromServer();

    };
}