#pragma once
// WARNING: if you include this file,
// place #endif to the end of your file
// or use #undef after #include or include this file after other files
// using defines for vove, vovec and coorder
#ifndef vove
#define vove std::pair<VirtualTile, VirtualTile>
#endif
#include <QPointer>

#include <utility>
enum packnum : uint8_t {
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
    interrupt_signal,
    already_registered
};

enum endnum : uint8_t {
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
    server_disconnected,
    ENDNUM_MAX
};

enum tatus : uint8_t {
    check_to_user,
    check_to_opponent,
    user_piece_eaten,
    opponent_piece_eaten,
    castling,
    promotion,
    just_new_turn,
    invalid_move
};

enum promnum {
    to_knight,
    to_bishop,
    to_castle,
    to_queen,
    no_promotion
};

#include <QDateTime>
inline QString curTime()
{
    return QDateTime::currentDateTime().toString("dd_MMM_hh_mm_ss_zzz");
}

#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QObject>
#include <QIcon>
inline void showBox(QString header,
                    QString text,
                    QMessageBox::Icon icon_type = QMessageBox::Information)
{
    QMessageBox msg_box;
    msg_box.setWindowTitle(header);
    msg_box.setWindowIcon(QIcon(":/images/app_icon"));
    msg_box.setText(text);
    msg_box.setIcon(icon_type);
    msg_box.addButton("Ok", QMessageBox::AcceptRole);
    msg_box.addButton("Donate", QMessageBox::RejectRole);
    QObject::connect(&msg_box, &QMessageBox::rejected, [&]() {
        QDesktopServices::openUrl(QUrl("https://www.buymeacoffee.com/mmd18", QUrl::TolerantMode));
    });
    msg_box.exec();
}

#include <stdexcept>
struct scoord {
    int x = 0;
    int y = 0;
    scoord(std::initializer_list<int> values) {
        if (values.size() != 2) {
            throw std::invalid_argument("Initializer list must contain exactly 2 values.");
        }
        auto it = values.begin();
        x = *it++;
        y = *it;
    }
    scoord() : x(0), y(0) {}
    scoord(const std::pair<int, int>& p) : x(p.first), y(p.second) {}
    bool isValid() {
        return x >= 0 && x <= 7 && y >= 0 && y <= 7;
    }
    bool operator == (const scoord& r) const {
        return x == r.x && y == r.y;
    }
    bool operator != (const scoord& r) const {
        return x != r.x || y != r.y;
    }
    bool operator<(const scoord& r) const {
        return (y != r.y ? y < r.y : x < r.x); // necessary for placing 
        // piece idxs on board in archiver
    }
    //operator std::pair<int, int>() const {
    //    return std::make_pair(x, y);
    //}
    //operator bool() {
    //    return isValid();
    //}
};

import bitremedy;
struct bitmove {
    bitremedy piece = {};
    bitremedy move = {};
    promnum promo = promnum::no_promotion;
};

#include <map>
extern std::map<char, promnum> promo_by_char;

extern std::map<promnum, char> char_by_promo;

#include <QVariant>
extern std::map<QString, QVariant> settings;

struct halfmove;

