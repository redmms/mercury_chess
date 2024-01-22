#pragma once
#include <utility>
#include <QDataStream>
#ifndef pove
#define pove std::pair<virtu, virtu>
#define povec const pove& 
#endif // !pove && !povec // WARNING: when you include this file,
// place #endif to the end of your file
// or use #undef after #include or include this file after other files
// using defines for pove, povec and coorder
#ifndef coorder
#define coorder const scoord& 
#endif // !coorder

enum package_ty : quint8 {
    registration,
    login,
    new_name,
    invite,
    invite_respond,
    move,
    chat_message,
    draw_suggestion,
    draw_respond,
    resignation,
    end_game,
    success,
    no_such_user,
    opponent_disconnected,
    none,
    wrong_password,
    user_offline,
    already_registered
};

enum endnum : quint8 {
    draw_by_agreement,
    draw_by_stalemate,
    user_wins,
    opponent_wins,
    user_resignation,
    opponent_resignation,
    user_out_of_time,
    opponent_out_of_time,
    draw_by_repetition, // FIX: need to handle
    draw_by_insufficient_material,  // FIX: need to handle
    opponent_disconnected_end,
    interrupt,
    server_disconnected
};

enum tatus : quint8 {
    check_to_user,
    check_to_opponent,
    user_piece_eaten,
    opponent_piece_eaten,
    castling,
    promotion,
    just_new_turn,
    invalid_move
};

struct scoord{
    int x = 0;
    int y = 0;
    
    bool operator != (const scoord right) const {
        return x != right.x || y != right.y;
    }
    bool operator == (const scoord right) const {
        return x == right.x && y == right.y;
    }
};

class Tile;
#include <QPointer>
struct virtu{
//    virtu(){tile = nullptr; color = false; name = 'e';}
    QPointer<Tile> tile;
    bool color = true;
    char name = 'K';
};

struct halfmove{
    pove move;
    char promo = 'e';
    bool castling = false;
    bool pass = false;
    bool turn = false;
};

#include <QDateTime>
inline QString curTime()
{
    return QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

#include <QMessageBox>
inline void showBox(QString header,
                    QString text,
                    QMessageBox::Icon icon_type = QMessageBox::Information)
{
    QMessageBox msg_box;
    msg_box.setWindowTitle(header);
    msg_box.setText(text);
    msg_box.setIcon(icon_type);
    msg_box.addButton("Ok", QMessageBox::AcceptRole);
    msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
    msg_box.exec();
}


//#include <QMessageBox>
//#include <functional>
//inline void showBox(QString header,
//                    QString text,
//                    QMessageBox::Icon icon_type = QMessageBox::Information,
//                    std::function<void()> accept_func = nullptr,
//                    std::function<void()> reject_func = nullptr)
//{
//    QMessageBox msg_box;
//    msg_box.setWindowTitle(header);
//    msg_box.setText(text);
//    msg_box.setIcon(icon_type);
//    msg_box.addButton("Ok", QMessageBox::AcceptRole);
//    msg_box.addButton("Hate you, but Ok", QMessageBox::RejectRole);
//    if (accept_func != nullptr){
//        QObject::connect(&msg_box, QMessageBox::AcceptRole, accept_func);
//    }
//    if (reject_func != nullptr){
//        QObject::connect(&msg_box, QMessageBox::RejectRole, reject_func);
//    }
//    msg_box.exec();
//}

