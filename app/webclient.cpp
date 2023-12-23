#include "webclient.h"
#include "C:\Qt_projects\multicolor_chess\src\game\board.h"
#include "C:\Qt_projects\multicolor_chess\src\game\tile.h"
#include "mainwindow.h"
#include <QWidget>
#include <iostream>
#include <QBuffer>
#include <QMessageBox>
#include <QTcpSocket>
#include "ui_mainwindow.h"

WebClient::WebClient(MainWindow* parent) :
    QObject(parent),
    mainwindow(parent),
    socket{},
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

void WebClient::initSocket()
{
    socket = new QTcpSocket(this); //FIX: how to destruct previous socket?
    connect(socket, &QTcpSocket::errorOccurred, [](QAbstractSocket::SocketError socketError){
        qDebug() << socketError;
    });
    connect(socket, &QTcpSocket::readyRead, this, &WebClient::readFromServer);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::disconnected, [&](){
        qDebug() << "Lost connection with server";

        if (mainwindow->game_active)
            mainwindow->endSlot(endnum::server_disconnected);

        QMessageBox msg_box;
        msg_box.setWindowTitle("Connection failed");
        msg_box.setText("Lost connection with server. What else did you expect from a noncommercial project?");
        msg_box.setIcon(QMessageBox::Critical);
        msg_box.exec();
        mainwindow->openTab(mainwindow->ui->pre_tab);
    });
}

void WebClient::checkConnection()
{
    if (socket == nullptr || !socket){
        initSocket();
        socket->connectToHost(/*"192.168.0.10"*/ "127.0.0.1", 49001);  // "145.249.226.3"
    }
    else{
        if (!socket->isOpen() || !socket->isValid() ){
            initSocket();
            socket->connectToHost(/*"192.168.0.10"*/ "127.0.0.1", 49001);  // "145.249.226.3"
        }
        else if (socket->state() != QAbstractSocket::ConnectedState) {
            socket->connectToHost(/*"192.168.0.10"*/ "127.0.0.1", 49001);  // "145.249.226.3"
        }
        else {
           qDebug() << "Socket is connected. Chill out.";
        }
    }
}


#include "webclient_pack_tools.h"

void WebClient::sendToServer(package_ty type, bool respond, QString message, scoord from, scoord to, char promotion_type)
{
    switch(type){
    case package_ty::registration:
        checkConnection();
        writePack(package_ty::registration);
        writePack(mainwindow->settings.value("user_name").toString());
        // FIX: add check of "user_name" setting exist. P.S. no need
        break;
    case package_ty::login:
        checkConnection();
        writePack(package_ty::login);
        writePack(mainwindow->settings.value("user_name").toString());
        break;
    case package_ty::invite:
        writePack(package_ty::invite);
        writePack(mainwindow->settings.value("opp_name").toString());
        writePack(mainwindow->settings.value("user_name").toString());
        writePack((quint8) (mainwindow->settings.value("time_setup").toInt()));
        writePack(!mainwindow->settings.value("match_side").toBool());
        writePack(mainwindow->user_pic);
        break;     
    case package_ty::invite_respond:
        writePack(package_ty::invite_respond);
        writePack(respond);
        if (respond)
            writePack(mainwindow->user_pic);
        break;
    case package_ty::move:
        writePack(package_ty::move);
        writePack(quint8(from.y * 8 + from.x));
        writePack(quint8(to.y * 8 + to.x));
        writePack(quint8(promotion_type));
        break;
    case package_ty::chat_message:
        writePack(package_ty::chat_message);
        writePack(message);
        break;
    case package_ty::draw_suggestion:
        writePack(package_ty::draw_suggestion);
        break;
    case package_ty::draw_respond:
        writePack(package_ty::draw_respond);
        writePack(respond);
        break;
    case package_ty::resignation:
        writePack(package_ty::resignation);
        break;
    case package_ty::end_game:
        writePack(package_ty::end_game);
        break;
    }

    auto bytes_written = socket->write(send_package);
    if (bytes_written == -1)
        qDebug() << "Couldn't write send_package to server";
    else if (bytes_written != send_package.size())
        qDebug() << "Bytes writen to server are not as wanted";
    send_package.clear();
    send_stream.device()->reset();
}

void WebClient::readFromServer()
{
    read_package = socket->readAll();
    if (read_package.isEmpty())
        qDebug() << "Package received from server is either empty or most likely there was an error";

    package_ty type;
    readPack(type);   
    switch(type){
    case package_ty::invite:  //show message box and send invite respond
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

        QMessageBox msg_box;
        msg_box.setWindowTitle("Invite to a game");
        msg_box.setText(opp_name + " invited you to play " + QString::number(time) + " minutes chess game.");
        msg_box.setIcon(QMessageBox::Information);
        msg_box.addButton("Open the board", QMessageBox::AcceptRole);
        msg_box.addButton("Reject invite", QMessageBox::RejectRole);
        connect(&msg_box, &QMessageBox::accepted, [&](){
            mainwindow->settings.setValue("opp_name", opp_name);
            if (!picture.isNull() && picture.size() == QSize{100, 100})
                mainwindow->opp_pic = picture;
            mainwindow->settings.setValue("match_side", side);
            mainwindow->settings.setValue("time_setup", int(time));
            mainwindow->startGame();
            sendToServer(package_ty::invite_respond, true);
        });
        connect(&msg_box, &QMessageBox::rejected, [&](){
            sendToServer(package_ty::invite_respond, false);
        });
        msg_box.exec();
        break;
    }
    case package_ty::invite_respond: // if true, load picture and start game else nothing (though in
        // future there should be a message box showing was it rejected or just waiting too long for an
        // answer)
    {
        qDebug() << "Invite respond received";
        bool respond;
        readPack(respond);
        if (respond){
            QPixmap picture;
            readPack(picture);
            mainwindow->opp_pic = picture;
            mainwindow->startGame();
        }
        emit endedReadingInvite();
        break;
    }
    case package_ty::move:
    {
        qDebug() << "Move received";
        quint8 from_k;
        readPack(from_k);
        scoord from{from_k % 8, from_k / 8};
        quint8 to_k;
        readPack(to_k);
        scoord to{to_k % 8, to_k / 8};
        mainwindow->board->halfMove(from, to);
        quint8 promo_int;
        readPack(promo_int);
        char promotion_type = char(promo_int);
        if (promotion_type != 'e'){
            bool color = !mainwindow->board->side;
            Board& board = *mainwindow->board;
            board[to.x][to.y]->setPiece(promotion_type, color);
        }
        break;
    }
    case package_ty::chat_message:
    {
        qDebug() << "Message received";
        QString message;
        readPack(message);
        QString name = mainwindow->settings.value("opp_name").toString();
        mainwindow->printMessage(name, false, message);
        break;
    }
    case package_ty::draw_suggestion: //show message box with opponent's name and draw suggestion
    {
        qDebug() << "Draw suggestion received";
        QMessageBox msg_box;
        msg_box.setWindowTitle("Draw suggestion");
        QString name = mainwindow->settings.value("opp_name").toString();
        msg_box.setText(name + " suggested you a draw.");
        //msg_box.setIcon(QMessageBox::Information);
        msg_box.addButton("Accept", QMessageBox::AcceptRole);
        msg_box.addButton("Decline", QMessageBox::RejectRole);
        connect(&msg_box, &QMessageBox::accepted, [this](){
            sendToServer(package_ty::draw_respond, true);
            mainwindow->endSlot(endnum::draw_by_agreement);
        });
        connect(&msg_box, &QMessageBox::rejected, [this](){
            sendToServer(package_ty::draw_respond, false);
        });
        msg_box.exec();
        break;
    }
    case package_ty::draw_respond: //if true stop the game as draw, else nothing
    {
        qDebug() << "Draw respond received";
        bool respond;
        readPack(respond);
        if (respond)
            mainwindow->endSlot(endnum::draw_by_agreement);
        break;
    }
    case package_ty::resignation: // mainwindow->endSlot(endnum::opponents_resignation
    {
        qDebug() << "Resignation from opponent received";
        mainwindow->endSlot(endnum::opponent_resignation);
        break;
    }
    case package_ty::no_such_user:
    {
        qDebug() << "No such user signal received";
        QMessageBox msg_box;
        msg_box.setWindowTitle("No such user");
        msg_box.setText("Player with this nickname wasn't found");
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
        break;
    }
    case package_ty::opponent_disconnected:
    {
        qDebug() << "Opponent disconnected signal received";
        mainwindow->endSlot(endnum::opponent_disconnected_end);
        break;
    }
    case package_ty::already_registered:
    {
        qDebug() << "Already registered signal received";
        QMessageBox msg_box;
        msg_box.setWindowTitle("Already registered");
        msg_box.setText("A user with this nickname is already registered. You need to work out a new one.");
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
        mainwindow->openTab(mainwindow->ui->login_tab);
        break;
    }
    case package_ty::success:
    {
        qDebug() << "Success signal received";
        QMessageBox msg_box;
        msg_box.setWindowTitle("Good news");
        msg_box.setText("Operation done successfuly.");
        msg_box.setIcon(QMessageBox::Information);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
        msg_box.exec();
        if (mainwindow->registering)
            mainwindow->openTab(mainwindow->ui->pre_tab);
        else
            mainwindow->openTab(mainwindow->ui->friend_connect_tab);
        break;
    }
    }
    read_package.clear();
    read_stream.device()->reset();
}
