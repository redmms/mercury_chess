#pragma once
#include "../app/local_types.h"
#include <QPointer>

class MainWindow;
class QTcpSocket;
class WebClient : public QObject
{
    Q_OBJECT

    QPointer<MainWindow> mainwindow;
    QPointer<QTcpSocket> socket;
    QByteArray read_package;
    QByteArray send_package;
    QDataStream read_stream;
    QDataStream send_stream;

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

public:
    // FIX: probaly should be explicit
    WebClient(MainWindow* parent = nullptr);
    ~WebClient();

    void initSocket();
    void checkConnection(packnum type);
    void connectToServer();
    void packFromSock(QTcpSocket* socket, QByteArray& received_package);
    void connectNewHost();
//    void sendRegistration();
//    void sendInvite();
//    void sendInviteRespond(bool respond);
//    void sendMove(scoord from, scoord to);
//    void sendChatMessage(QString message);
//    void sendDrawSuggestion();
//    void sendDrawRespond(bool respond);
//    void sendResignation();
//    void sendEndGame();

signals:
    void endedReadingInvite();

public slots:
    void sendToServer(packnum type, bool respond = false, QString message = "", scoord from = {}, scoord to = {}, char promotion_type = 'e');
    void readFromServer();

};