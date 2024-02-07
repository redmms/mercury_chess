#pragma once
import bitremedy;
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
    server_disconnected,
    ENDNUM_MAX
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
    bool isValid(){
        return x >=0 && x <= 7 && y >= 0 && y <= 7;
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
    operator std::pair<int, int>() const {
        return std::make_pair(x, y);
    }
    operator bool() {
        return isValid();
    }
};

class Tile;
#include <QPointer>
struct virtu{
//    virtu(){tile = nullptr; color = false; name = 'e';}
    QPointer<Tile> tile;
    bool color = true;
    char name = 'K'; // this way you will soon know if something went wrong
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
    return QDateTime::currentDateTime().toString("dd_MMM_hh_mm_ss_zzz");
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

enum promo_ty {
    to_knight,
    to_bishop,
    to_castle,
    to_queen,
    no_promotion
};

#include <map>
static std::map<char, promo_ty> promo_by_char{
    {'N', to_knight},
    {'B', to_bishop},
    {'R', to_castle},
    {'Q', to_queen},
    {'e', no_promotion}
};

static std::map<promo_ty, char> char_by_promo{
    {to_knight, 'N'},
    {to_bishop, 'B'},
    {to_castle, 'R'},
    {to_queen, 'Q'},
    {no_promotion, 'e'}
};

struct bitmove {
    bitremedy piece = {};
    bitremedy move = {};
    promo_ty promo = promo_ty::no_promotion;
};