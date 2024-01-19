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
#include <QEventLoop>

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
        qDebug() << curTime() << "read_stream error!";
    if (send_stream.status() != QDataStream::Ok)
        qDebug() << curTime() << "send_stream error!";
}

void WebClient::initSocket()
{
    socket.reset(new QTcpSocket(this)); //FIX: how to destruct previous socket?
    // FIX: will note QScopedPointer deleter mess with socket->deleteLater()?
    connect(socket.data(), &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError socketError){
        if (socket->state() == QAbstractSocket::UnconnectedState){
            qDebug() << curTime() << "Couldn't connect to server:";
            qDebug() << curTime() << socketError;
            showBox("No connection",
                    "You are offline. Contact me by mmd18cury@yandex.ru to start the server.",
                    QMessageBox::Warning);
//            mainwindow->settings.setValue("game_regime", "friend_offline");
//            mainwindow->startGame();
        }
        else if (socket->state() != QAbstractSocket::ConnectedState){
            qDebug() << curTime() << "Error occured while trying to connect to server:";
            qDebug() << curTime() << socketError;
            showBox("No connection",
                    "No connection, some error occured. Contact me by mmd18cury@yandex.ru.",
                    QMessageBox::Warning);
        }
        else{
            qDebug() << curTime() << "Socket error signal received, but socket state is connected:";
            qDebug() << curTime() << socketError;
        }
    });
    connect(socket.data(), &QTcpSocket::connected, [&](){
        qDebug() << curTime() << "Connected to server.";
    });
    connect(socket.data(), &QTcpSocket::readyRead, [&](){
            while(socket->bytesAvailable() > 0){
                this->readFromServer();
            }
    });
    connect(socket.data(), &QTcpSocket::disconnected, [&](){
        qDebug() << curTime() << "Lost connection with server";
        socket->deleteLater();
        if (mainwindow->game_active)
            mainwindow->endSlot(endnum::server_disconnected);
        showBox("Connection failed",
                "Lost connection with server, log in again. What else did you expect from a noncommercial project?",
                QMessageBox::Critical);
        mainwindow->openTab(mainwindow->ui->pre_tab);
    });
}

void WebClient::checkConnection(package_ty type)
{
    if (!socket){
        if (type != package_ty::registration && type != package_ty::login)
            qDebug() << curTime() << "Warning: you need to register or log in before sending any data to the server";
        initSocket();
        connectToServer();
    }
    else if (!socket->isValid() || !socket->isOpen()){
        initSocket();
        connectToServer();
    }
    else if (socket->state() == QAbstractSocket::UnconnectedState){
        connectToServer();
    }
    else{
        //auto state_copy = socket->state();
        qDebug() << curTime()
                 << "Socket is fine. Connection also seems to be fine. Socket state is "
                 << socket->state();
    }
}

void WebClient::connectToServer()
// socket parameter should be a valid QTcpSocket*
{
//    if (socket->state() == QAbstractSocket::UnconnectedState)

    socket->connectToHost("40.113.33.140", 49001);  // /*"192.168.0.10"*/ "127.0.0.1"
    //socket->connectToHost("127.0.0.1", 49001);  // /*"192.168.0.10"*/ "127.0.0.1"

    //auto copy_state = socket->state();
}

void WebClient::packFromSock(QTcpSocket *socket, QByteArray &read_package)
{
    //auto avail_copy = socket->bytesAvailable();
    while(socket->bytesAvailable() < 2){
        if (!socket->waitForReadyRead()) {
            qDebug() << curTime() << "waitForReadyRead() timed out";
            return;
        }
    }
    read_package = socket->read(2);
    //QDataStream package_stream(read_package);
    quint16 pack_size;
    read_stream >> pack_size;
    qDebug() << curTime() << "Received package size supposed to be" << pack_size;
    // else can be done: (((pack_size = 0) &= buffer[0]) <<= 8) &= buffer[1]
    // but it will depend on endiannes;

    int need_to_add = pack_size - read_package.size();
    if (socket->bytesAvailable() >= need_to_add){
        qDebug() << curTime() << "Full package arrived";
        read_package.append(socket->read(need_to_add));
    }
    else{
        qDebug() << curTime() << "Only a part of package arrived. Waiting for other parts";
        while (socket->bytesAvailable() < need_to_add) {
            if (!socket->waitForReadyRead()) {
                qDebug() << curTime() << "Error waiting for more data";
                return;
            }
        }
        read_package.append(socket->read(need_to_add));
        qDebug() << curTime() << "Package is fulfilled";
    }

    if (read_package.isEmpty()){
        qDebug() << curTime() << "Received package was empty or most likely there was an error";
        return;
    }
    else{
        qDebug() << curTime() << "Received package size is" << read_package.size();
    }
    read_stream.device()->reset();
}

#include "webclient_pack_tools.h"

void WebClient::sendToServer(package_ty type, bool respond, QString message, scoord from, scoord to, char promotion_type)
{
    quint16 pack_size = 0;
    send_stream << pack_size;
    checkConnection(type);
    switch(type){
    case package_ty::registration:
        writePack(package_ty::registration);
        writePack(mainwindow->settings.value("user_name").toString());
        writePack(mainwindow->settings.value("user_pass").toByteArray());
        // FIX: add check of "user_name" setting exist. P.S. no need
        break;
    case package_ty::login:
        writePack(package_ty::login);
        writePack(mainwindow->settings.value("user_name").toString());
        writePack(mainwindow->settings.value("user_pass").toByteArray());
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
    send_stream.device()->seek(0);
    pack_size = (quint16) send_package.size();
    send_stream << pack_size;

//    QByteArray little_copy{};
//    for (int l = 0; l < send_package.size(); l += 10){
//        for (int r = l; r < l + 10 && r < send_package.size(); r++){

//            little_copy.push_back(send_package[r]);
//        }
//        auto bytes_written = socket->write(little_copy);
//        if (!socket->waitForBytesWritten()) // FIX: it may cause the problem with pictures
//            qDebug() << curTime() << "Couldn't wait for bytes to be written";
//        little_copy.clear();
//    }

    auto bytes_written = socket->write(send_package);
    if (!socket->waitForBytesWritten(10000)) // FIX: it may cause the problem with pictures
        qDebug() << curTime() << "Couldn't wait for bytes to be written";
    if (bytes_written == -1)
        qDebug() << curTime() << "Couldn't write send_package to server";
    else if (bytes_written != send_package.size())
        qDebug() << curTime() << "Bytes writen to server are not as wanted";

    send_package.clear();
    send_stream.device()->reset();
}

void WebClient::readFromServer()
{
    packFromSock(socket.data(), read_package);
    quint16 skip;
    read_stream >> skip;

    package_ty type;
    readPack(type);   
    switch(type){
    case package_ty::invite:  //show message box and send invite respond
    {
        qDebug() << curTime() << "Invite received";
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
            else
                mainwindow->opp_pic = mainwindow->default_pic;
            mainwindow->settings.setValue("match_side", side);
            mainwindow->settings.setValue("time_setup", int(time));
            mainwindow->settings.setValue("game_regime", "friend_online");
            mainwindow->startGame();
            sendToServer(package_ty::invite_respond, true);
        });
        connect(&msg_box, &QMessageBox::rejected, [&](){
            sendToServer(package_ty::invite_respond, false);
        });
        msg_box.exec();
        break;
    }
    case package_ty::invite_respond:
        // if true, load picture and start game else nothing (though in
        // future there should be a message box showing was it rejected or just waiting too long for an
        // answer)
    {
        qDebug() << curTime() << "Invite respond received";
        bool respond;
        readPack(respond);
        if (respond){
            QPixmap picture;
            readPack(picture);
            if (!picture.isNull() && picture.size() == QSize{100, 100})
                mainwindow->opp_pic = picture;
            else
                mainwindow->opp_pic = mainwindow->default_pic;
            mainwindow->settings.setValue("game_regime", "friend_online");
            mainwindow->startGame();
        }
        emit endedReadingInvite();
        break;
    }
    case package_ty::move:
    {
        qDebug() << curTime() << "Move received";
        quint8 from_k;
        readPack(from_k);
        scoord from{from_k % 8, from_k / 8};
        quint8 to_k;
        readPack(to_k);
        scoord to{to_k % 8, to_k / 8};
        if (!mainwindow->board){
            qDebug() << curTime() << "ERROR: trying to write a move to nonexisting board";
            return;
        }
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
        qDebug() << curTime() << "Message received";
        QString message;
        readPack(message);
        QString name = mainwindow->settings.value("opp_name").toString();
        mainwindow->printMessage(name, false, message);
        break;
    }
    case package_ty::draw_suggestion: //show message box with opponent's name and draw suggestion
    {
        qDebug() << curTime() << "Draw suggestion received";
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
        qDebug() << curTime() << "Draw respond received";
        bool respond;
        readPack(respond);
        if (respond)
            mainwindow->endSlot(endnum::draw_by_agreement);
        break;
    }
    case package_ty::resignation: // mainwindow->endSlot(endnum::opponents_resignation
    {
        qDebug() << curTime() << "Resignation from opponent received";
        mainwindow->endSlot(endnum::opponent_resignation);
        break;
    }
    case package_ty::no_such_user:
    {
        qDebug() << curTime() << "No such user signal received";
        showBox("No such user",
                "Player with this nickname wasn't found",
                QMessageBox::Warning);
        emit endedReadingInvite();
        break;
    }
    case package_ty::opponent_disconnected:
    {
        qDebug() << curTime() << "Opponent disconnected signal received";
        mainwindow->endSlot(endnum::opponent_disconnected_end);
        break;
    }
    case package_ty::already_registered:
    {
        qDebug() << curTime() << "Already registered signal received";
        showBox("Already registered",
                "A user with this nickname is already registered. You need to work out a new one.",
                QMessageBox::Warning);
        mainwindow->openTab(mainwindow->ui->login_tab);
        break;
    }
    case package_ty::success:
    {
        qDebug() << curTime() << "Success signal received";
        showBox("Good news",
                "Operation done successfuly.");

        if (mainwindow->login_regime == 2){
            mainwindow->openTab(mainwindow->ui->pre_tab);
            mainwindow->ui->login_password->clear();
        }
        else if (mainwindow->login_regime == 1){
            mainwindow->openTab(mainwindow->ui->friend_connect_tab);
            mainwindow->ui->menuOnline->setEnabled(true);
            mainwindow->ui->actionProfile->setEnabled(true);
            mainwindow->ui->login_password->clear();
        }
        else if (mainwindow->login_regime == 3){
            mainwindow->login_regime = 1;
            mainwindow->ui->login_password->clear();
        }
        break;
    }
    case package_ty::wrong_password:
    {
        showBox("Wrong password",
                "I understand you perfectly, I also have a bad memory. Should I advise you some memory pills?",
                QMessageBox::Warning);
        break;
    }
    case package_ty::user_offline:
    {
        showBox("User offline",
                "Suggested user is offline.");
        emit endedReadingInvite();
        break;
    }
    }
    read_package.clear();
    read_stream.device()->reset();
}
