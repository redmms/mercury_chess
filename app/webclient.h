#ifndef WEBCLIENT_H
#define WEBCLIENT_H
#include <QByteArray>
#include <QDataStream>
#include "../game/local_types.h"
#include "qbytearray.h"
#include "qdatastream.h"
#include "qobject.h"
#include "qstring.h"
#include <QTcpSocket>
#include <QPointer>

class MainWindow;
class QPixmap;

class WebClient : public QObject
{
	Q_OBJECT

    QPointer<MainWindow> mainwindow;
    QPointer<QTcpSocket> socket;
	QByteArray read_package;
	QByteArray send_package;
	QDataStream read_stream;
	QDataStream send_stream;

	void writePack(package_ty);
	void writePack(quint8);
	void writePack(bool);
	void writePack(const QByteArray&);
	void writePack(const QString&);
	void writePack(const QPixmap&);

	void readPack(package_ty&);
	void readPack(quint8&);
	void readPack(bool&);
	void readPack(QByteArray&);
	void readPack(QString&);
	void readPack(QPixmap&);

public:
	// FIX: probaly should be explicit
	WebClient(MainWindow* parent = nullptr);

    void initSocket();
    void checkConnection(package_ty type);
    void connectToServer();
    void packFromSock(QTcpSocket* socket, QByteArray& received_package);
    void connectNewHost();
//	void sendRegistration();
//	void sendInvite();
//	void sendInviteRespond(bool respond);
//	void sendMove(scoord from, scoord to);
//	void sendChatMessage(QString message);
//	void sendDrawSuggestion();
//	void sendDrawRespond(bool respond);
//	void sendResignation();
//	void sendEndGame();

signals:
	void endedReadingInvite();

public slots:
    void sendToServer(package_ty type, bool respond = false, QString message = "", scoord from = {}, scoord to = {}, char promotion_type = 'e');
	void readFromServer();

};

#endif // WEBCLIENT_H
