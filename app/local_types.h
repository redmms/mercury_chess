#pragma once
// WARNING: if you include this file,
// place #endif to the end of your file
// or use #undef after #include or include this file after other files
// using defines for vove, vovec and coorder
#ifndef vove
#define vove std::pair<VirtualTile, VirtualTile>
#endif

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
    bool operator < (const scoord& r) const {
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

#include <QBuffer>
#include <QBitmap>
inline void setPic(QString par, const QPixmap& pic) {
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    pic.save(&buffer, "PNG");
    settings[par].setValue(arr);
}

inline QPixmap getPic(QString par) {
    QByteArray arr = settings[par].toByteArray();
    QPixmap pic;
    pic.loadFromData(arr, "PNG");
    return pic;
}

inline void setBMap(QString par, const QBitmap& bmap) {
    settings[par] = QVariant(bmap);
}

inline QBitmap getBMap(QString par) {
    return qvariant_cast<QBitmap>(settings[par]);
}

inline unsigned char arrToChar(const bool(&arr)[6])
{
    unsigned char c = 0;
    for (int i = 5; i >= 0; i--) {
        c <<= 1;
        c |= arr[i];
    }
    return c;
}

inline void charToArr(unsigned char c, bool(&arr)[6])
{
    for (int i = 0; i < 6; i++) {
        arr[i] = c & true;
        c >>= 1;
    }
}

#include <QString>
inline QString coordToString(scoord coord)
{
    return QString(char('a' + coord.x)) + QString::number(coord.y + 1);
}

#include <string>
inline scoord stringToCoord(QString str)
{
    std::string s = str.toStdString();
    return { s[0] - 'a', s[1] - '0' - 1};
}

extern scoord a1;
extern scoord a2;
extern scoord a3;
extern scoord a4;
extern scoord a5;
extern scoord a6;
extern scoord a7;
extern scoord a8;
extern scoord b1;
extern scoord b2;
extern scoord b3;
extern scoord b4;
extern scoord b5;
extern scoord b6;
extern scoord b7;
extern scoord b8;
extern scoord c1;
extern scoord c2;
extern scoord c3;
extern scoord c4;
extern scoord c5;
extern scoord c6;
extern scoord c7;
extern scoord c8;
extern scoord d1;
extern scoord d2;
extern scoord d3;
extern scoord d4;
extern scoord d5;
extern scoord d6;
extern scoord d7;
extern scoord d8;
extern scoord e1;
extern scoord e2;
extern scoord e3;
extern scoord e4;
extern scoord e5;
extern scoord e6;
extern scoord e7;
extern scoord e8;
extern scoord f1;
extern scoord f2;
extern scoord f3;
extern scoord f4;
extern scoord f5;
extern scoord f6;
extern scoord f7;
extern scoord f8;
extern scoord g1;
extern scoord g2;
extern scoord g3;
extern scoord g4;
extern scoord g5;
extern scoord g6;
extern scoord g7;
extern scoord g8;
extern scoord h1;
extern scoord h2;
extern scoord h3;
extern scoord h4;
extern scoord h5;
extern scoord h6;
extern scoord h7;
extern scoord h8;
