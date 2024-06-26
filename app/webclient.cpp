#include "webclient.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chat.h"
#include "../game/board.h"
#include <QTcpSocket>
#include <QBuffer>
using namespace std;

namespace mmd
{
    WebClient::WebClient(MainWindow* parent) :
        QObject(parent),
        mainwindow(parent),
        socket(nullptr),
        read_package{},
        send_package{},
        read_stream(&read_package, QIODevice::ReadOnly),
        send_stream(&send_package, QIODevice::WriteOnly)
    {
        read_stream.setVersion(QDataStream::Qt_5_15);
        send_stream.setVersion(QDataStream::Qt_5_15);
        if (read_stream.status() != QDataStream::Ok)
            qDebug() << "read_stream error!";
        if (send_stream.status() != QDataStream::Ok)
            qDebug() << "send_stream error!";
    }

    WebClient::~WebClient() {
        if (socket) {
            socket->disconnect();
        }
        delete socket;
    }

    void WebClient::initSocket()
    {
        if (socket) {
            socket->deleteLater();
            qDebug() << "Previous socket in queue for deletion";
        }
        socket = (new QTcpSocket(this));
        qDebug() << "Socket initialized";
        connect(socket, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError socketError) {
            if (socket->state() == QAbstractSocket::UnconnectedState) {
                qDebug() << "Couldn't connect to the server:";
                qDebug() << socketError;
                showBox("No connection",
                    "You are offline.",
                    QMessageBox::Warning);
            }
            else if (socket->state() != QAbstractSocket::ConnectedState) {
                qDebug() << "Error occured while trying to connect to server:";
                qDebug() << socketError;
                showBox("No connection",
                    "An error occured. Contact me by mmd18cury@yandex.ru.",
                    QMessageBox::Warning);
            }
            else {
                qDebug() << "Socket error signal received, but socket state is connected:";
                qDebug() << socketError;
            }
            });
        connect(socket, &QTcpSocket::connected, [&]() {
            qDebug() << "Connected to server.";
            });
        connect(socket, &QTcpSocket::readyRead, [&]() {
            while (socket->bytesAvailable() > 0) {
                this->readFromServer();
            }
            });
        connect(socket, &QTcpSocket::disconnected, [&]() {
            qDebug() << "Lost connection with server";
            if (mainwindow->game_active)
                mainwindow->endSlot(endnum::server_disconnected);
            showBox("Connection failed",
                "Lost connection with server, log in again. What else did you expect from a noncommercial project?",
                QMessageBox::Critical);
            mainwindow->openTab(mainwindow->ui->pre_tab);
            });
        connect(socket, &QTcpSocket::destroyed, [&]() {
            qDebug() << "Socket destroyed.";
            });
    }

    bool WebClient::checkConnection(packnum type)
    {
        if (!socket) {
            // forgot to initialize a socket? Go check your code :)
            if (type == new_name) {
                showBox("Can't change online nickname",
                    "You are offline, only local nickname has been changed. "
                    "To change online nickname you will need to enter account. ");
            }
            else {
                showBox("Oops, you forgot to log in",
                    "You need to register and log in before sending any data to the server",
                    QMessageBox::Warning);
            }
            return false;
        }
        else if (!socket->isValid()) {
            qDebug() << "Socket wasn't valid, connecting.";
            return connectToServer();
        }
        else if(!socket->isOpen()){
            qDebug() << "Socket wasn't open, connecting.";
            return connectToServer();
        }
        else if (socket->state() == QAbstractSocket::UnconnectedState) {
            qDebug() << "Socket wasn't connected, connecting.";
            return connectToServer();
        }
        else {
            qDebug() << "Socket is fine. Connection also seems to be fine. Socket state is "
                << socket->state();
            return true;
        }
    }

    bool WebClient::connectToServer()
    {
        QString address = settings[ip_address_e].toString();
        int port = settings[port_address_e].toInt();
        if (socket) {
            socket->connectToHost(address, port);
            return socket->waitForConnected(3000);
        }
        else {
            qDebug() << "Socket was uninitialized before connecting to the server";
            return false;
        }
    }

    void WebClient::connectNewHost()
    {
        if (!socket) {
            initSocket();
        }
        if (connectToServer()) {
            showBox("Success", "New connection established. Now log in.");
        }
        else {
            showBox("Fail", "Couldn't establish new connection. Close the app and watch log file too see socket connection error.");
        }
    }

    void WebClient::packFromSock(QTcpSocket* socket, QByteArray& read_package)
    {
        while (socket->bytesAvailable() < 2) {
            if (!socket->waitForReadyRead()) {
                qDebug() << "waitForReadyRead() timed out";
                return;
            }
        }
        read_package = socket->read(2);
        quint16 pack_size;
        read_stream >> pack_size;
        qDebug() << "Received package size supposed to be" << pack_size;
        // else can be done: (((pack_size = 0) &= buffer[0]) <<= 8) &= buffer[1]
        // but it will depend on endiannes;

        int need_to_add = pack_size - read_package.size();
        if (socket->bytesAvailable() >= need_to_add) {
            qDebug() << "Full package arrived";
            read_package.append(socket->read(need_to_add));
        }
        else {
            qDebug() << "Only a part of package arrived. Waiting for other parts";
            while (socket->bytesAvailable() < need_to_add) {
                if (!socket->waitForReadyRead()) {
                    qDebug() << "Error waiting for more data";
                    return;
                }
            }
            read_package.append(socket->read(need_to_add));
            qDebug() << "Package is fulfilled";
        }

        if (read_package.isEmpty()) {
            qDebug() << "Received package was empty or most likely there was an error";
            return;
        }
        else {
            qDebug() << "Received package size is" << read_package.size();
        }
        read_stream.device()->reset();
    }

    void WebClient::sendToServer(packnum type, bool respond, QString message, scoord from, scoord to, char promotion_type)
    {
        quint16 pack_size = 0;
        send_stream << pack_size;
        if ((type == registration || type == login) && !socket) {
            initSocket();
            connectToServer();
        }
        if (!checkConnection(type)) {
            qDebug() << "Couldn't send data to server. Data type is " << int(type);
            return;
        }
        switch (type) {
        case packnum::registration:
            writePack(packnum::registration);
            writePack(settings[user_name_e].toString());
            writePack(settings[user_pass_e].toByteArray());
            break;
        case packnum::login:
            writePack(packnum::login);
            writePack(settings[user_name_e].toString());
            writePack(settings[user_pass_e].toByteArray());
            break;
        case packnum::new_name:
            writePack(packnum::new_name);
            writePack(message);
            break;
        case packnum::invite:
            writePack(packnum::invite);
            writePack(settings[opp_name_e].toString());
            writePack(settings[user_name_e].toString());
            writePack((quint8)(settings[time_setup_e].toInt()));
            writePack(!settings[match_side_e].toBool());
            writePack(getPic(user_pic_e));
            break;
        case packnum::invite_respond:
            writePack(packnum::invite_respond);
            writePack(respond);
            if (respond)
                writePack(getPic(user_pic_e));
            break;
        case packnum::move_pack:
            writePack(packnum::move_pack);
            writePack(quint8(from.y * 8 + from.x));
            writePack(quint8(to.y * 8 + to.x));
            writePack(quint8(promotion_type));
            break;
        case packnum::chat_message:
            writePack(packnum::chat_message);
            writePack(message);
            break;
        case packnum::draw_suggestion:
            writePack(packnum::draw_suggestion);
            break;
        case packnum::draw_respond:
            writePack(packnum::draw_respond);
            writePack(respond);
            break;
        case packnum::resignation:
            writePack(packnum::resignation);
            break;
        case packnum::end_game:
            writePack(packnum::end_game);
            break;
        case packnum::interrupt_signal:
            writePack(packnum::interrupt_signal);
            break;
        }
        send_stream.device()->seek(0);
        pack_size = (quint16)send_package.size();
        send_stream << pack_size;

        // Use for testing:
        //    QByteArray little_copy{};
        //    for (int l = 0; l < send_package.size(); l += 10){
        //        for (int r = l; r < l + 10 && r < send_package.size(); ++r){
        //            little_copy.push_back(send_package[r]);
        //        }
        //        auto bytes_written = socket->write(little_copy);
        //        if (!socket->waitForBytesWritten())
        //            qDebug() << "Couldn't wait for bytes to be written";
        //        little_copy.clear();
        //    }

        auto bytes_written = socket->write(send_package);
        if (!socket->waitForBytesWritten(10000))
            qDebug() << "Couldn't wait for bytes to be written";
        if (bytes_written == -1)
            qDebug() << "Couldn't write send_package to server";
        else if (bytes_written != send_package.size())
            qDebug() << "Bytes writen to server are not as wanted";

        send_package.clear();
        send_stream.device()->reset();
    }

    void WebClient::readFromServer()
    {
        packFromSock(socket, read_package);
        quint16 skip;
        read_stream >> skip;

        packnum type;
        readPack(type);
        switch (type) {
        case packnum::invite:  // show message box and send invite respond
        {
            qDebug() << "Invite received";
            QString user_name;
            readPack(user_name);
            QString opp_name;
            readPack(opp_name);
            quint8 time;
            readPack(time);
            bool side;
            readPack(side);
            QPixmap picture;
            readPack(picture);

            QMessageBox msg_box(mainwindow);
            msg_box.setWindowTitle("Invite to a game");
            msg_box.setText(opp_name + " invited you to play " + QString::number(time) + " minutes chess game.");
            msg_box.setIcon(QMessageBox::Information);
            msg_box.addButton("Open the board", QMessageBox::AcceptRole);
            msg_box.addButton("Reject invite", QMessageBox::RejectRole);
            connect(&msg_box, &QMessageBox::accepted, [&]() {
                settings[opp_name_e].setValue(opp_name);
                int width = settings[pic_w_e].toInt();
                int height = settings[pic_h_e].toInt();
                if (!picture.isNull() && picture.size() == QSize{ width, height })
                    setPic(opp_pic_e, picture);
                else
                    setPic(opp_pic_e, getPic(def_pic_e));
                settings[match_side_e].setValue(side);
                settings[time_setup_e].setValue(int(time));
                mainwindow->startGame(friend_online);
                sendToServer(packnum::invite_respond, true);
                });
            connect(&msg_box, &QMessageBox::rejected, [&]() {
                sendToServer(packnum::invite_respond, false);
                });
            msg_box.exec();
            break;
        }
        case packnum::invite_respond:
            // if true, load picture and start game else nothing (though in
            // future there should be a message box showing was it rejected or just waiting too long for an
            // answer)
        {
            qDebug() << "Invite respond received";
            bool respond;
            readPack(respond);
            if (respond) {
                QPixmap picture;
                readPack(picture);
                int width = settings[pic_w_e].toInt();
                int height = settings[pic_h_e].toInt();
                if (!picture.isNull() && picture.size() == QSize{ width, height })
                    setPic(opp_pic_e, picture);
                else
                    setPic(opp_pic_e, getPic(def_pic_e));
                mainwindow->startGame(friend_online);
            }
            emit endReadingInvite();
            break;
        }
        case packnum::move_pack:
        {
            qDebug() << "Move received";
            quint8 from_k;
            readPack(from_k);
            scoord from{ from_k % 8, from_k / 8 };
            quint8 to_k;
            readPack(to_k);
            scoord to{ to_k % 8, to_k / 8 };
            if (!mainwindow->board) {
                qDebug() << "ERROR: trying to write a move to nonexisting board";
                return;
            }
            quint8 promo;
            readPack(promo);
            mainwindow->board->savingHalfMove(from, to, promo);
            break;
        }
        case packnum::chat_message:
        {
            qDebug() << "Message received";
            QString message;
            readPack(message);
            QString name = settings[opp_name_e].toString();
            if (mainwindow->game_active)
                mainwindow->chat->printMessage(name, false, message);
            break;
        }
        case packnum::draw_suggestion: //show message box with opponent's name and draw suggestion
        {
            qDebug() << "Draw suggestion received";
            QMessageBox msg_box(mainwindow);
            msg_box.setWindowTitle("Draw suggestion");
            QString name = settings[opp_name_e].toString();
            msg_box.setText(name + " suggested you a draw.");
            msg_box.addButton("Accept", QMessageBox::AcceptRole);
            msg_box.addButton("Decline", QMessageBox::RejectRole);
            connect(&msg_box, &QMessageBox::accepted, [this]() {
                sendToServer(packnum::draw_respond, true);
                mainwindow->endSlot(endnum::draw_by_agreement);
                });
            connect(&msg_box, &QMessageBox::rejected, [this]() {
                sendToServer(packnum::draw_respond, false);
                });
            msg_box.exec();
            break;
        }
        case packnum::draw_respond: //if true stop the game as draw, else nothing
        {
            qDebug() << "Draw respond received";
            bool respond;
            readPack(respond);
            if (respond)
                mainwindow->endSlot(endnum::draw_by_agreement);
            break;
        }
        case packnum::resignation:
        {
            qDebug() << "Resignation from opponent received";
            mainwindow->endSlot(endnum::opponent_resignation);
            break;
        }
        case packnum::no_such_user:
        {
            qDebug() << "No such user signal received";
            showBox("No such user",
                "Player with this nickname wasn't found",
                QMessageBox::Warning);
            emit endReadingInvite();
            break;
        }
        case packnum::opponent_disconnected:
        {
            qDebug() << "Opponent disconnected signal received";
            mainwindow->endSlot(endnum::opponent_disconnected_end);
            break;
        }
        case packnum::already_registered:
        {
            qDebug() << "Already registered signal received";
            showBox("Already registered",
                "A user with this nickname is already registered. You need to work out a new one.",
                QMessageBox::Warning);
            mainwindow->openTab(mainwindow->ui->login_tab);
            break;
        }
        case packnum::success:
        {
            qDebug() << "Success signal received";
            showBox("Good news",
                "Operation done successfuly.");

            if (mainwindow->login_regime == reg_r) { // reg
                mainwindow->openTab(mainwindow->ui->pre_tab);
            }
            else if (mainwindow->login_regime == log_r) { // log
                mainwindow->openTab(mainwindow->ui->friend_connect_tab);
                mainwindow->ui->menuOnline->setEnabled(true);
            }
            else if (mainwindow->login_regime == guest_r) { // guest
                mainwindow->login_regime = log_r;
            }

            mainwindow->ui->login_password->clear();
            break;
        }
        case packnum::wrong_password:
        {
            showBox("Wrong password",
                "I understand you perfectly, I also have a bad memory. Should I advise you some memory pills?",
                QMessageBox::Warning);
            break;
        }
        case packnum::user_offline:
        {
            showBox("User offline",
                "Suggested user is offline.");
            emit endReadingInvite();
            break;
        }
        }
        read_package.clear();
        read_stream.device()->reset();
    }
}  // namespace mmd
