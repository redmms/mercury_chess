#pragma once
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QLayoutItem>
#include <utility>
#include <cstdint>
import bitremedy;

class QBitmap;
class QPixmap;
class QIcon;
namespace mmd 
{
    class VirtualTile;
    using vove = std::pair<VirtualTile, VirtualTile>;

    enum lognum : uint8_t 
    {
        log_r,
        reg_r,
        guest_r,
        none_r
    };

    enum setnum : uint8_t 
    {
        user_name_e,
        opp_name_e,
        time_setup_e,
        match_side_e,
        game_regime_e,
        def_port_e,
        port_address_e,
        def_address_e,
        ip_address_e,
        max_nick_e,
        pic_w_e,
        pic_h_e,
        def_pic_e,
        user_pic_e,
        opp_pic_e,
        pic_mask_e,
        user_pass_e
    };

    enum soundnum : uint8_t 
    {
        move_s,
        user_eaten_s,
        opp_eaten_s,
        castling_s,
        promotion_s,
        check_to_user_s,
        check_to_opp_s,
        invalid_move_s,
        lose,
        win,
        draw,
        start
    };

    enum packnum : uint8_t 
    {
        registration,
        login,
        new_name,
        invite,
        invite_respond,
        move_pack,
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

    enum endnum : uint8_t 
    {
        draw_by_agreement,
        draw_by_stalemate,
        user_wins,
        opponent_wins,
        user_resignation,
        opponent_resignation,
        user_out_of_time,
        opponent_out_of_time,
        draw_by_repetition, // TODO: need to handle
        draw_by_insufficient_material,  // TODO: need to handle
        opponent_disconnected_end,
        interrupt,
        server_disconnected,
        ENDNUM_MAX
    };

    enum estatus : uint8_t 
    {
        check_to_user,
        check_to_opponent,
        user_piece_eaten,
        opponent_piece_eaten,
        castling,
        promotion,
        just_new_turn,
        invalid_move
    };

    enum promnum 
    {
        to_knight,
        to_bishop,
        to_castle,
        to_queen,
        no_promotion
    };

    inline QString curTime()
    {
        return QDateTime::currentDateTime().toString("dd_MMM_hh_mm_ss_zzz");
    }

    void showBox(QString header,
        QString text,
        QMessageBox::Icon icon_type = QMessageBox::Information);

    struct scoord 
    {
        int x = 0;
        int y = 0;
        scoord(std::initializer_list<int> values) {
            if (values.size() != 2) {
                throw std::invalid_argument("Initializer list must contain exactly 2 values.");
            }
            auto it = values.begin();
            x = *it;
            y = *++it;
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

    struct bitmove 
    {
        fn::bitremedy piece = {};
        fn::bitremedy move = {};
        promnum promo = promnum::no_promotion;
    };

    extern const std::map<char, promnum> promo_by_char;

    extern const std::map<promnum, char> char_by_promo;

    extern std::map<setnum, QVariant> settings;

    struct halfmove;

    void setPic(setnum par, const QPixmap& pic);

    QPixmap getPic(setnum par);

    void setBMap(setnum par, const QBitmap& bmap);

    QBitmap getBMap(setnum par);

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
        for (int i = 0; i < 6; ++i) {
            arr[i] = c & true;
            c >>= 1;
        }
    }

    QString coordToString(scoord coord);

    scoord stringToCoord(QString str);

    template <typename OLD, typename NEW>
        //requires(std::is_base_of_v<QObject, OLD> && std::is_base_of_v<QObject, NEW>)
    void replaceOld(NEW young, OLD& old) {
        if (!young || !old) {
            qDebug() << "WARNING: wrong arguments of replaceOld()";
            return;
        }
        QLayout* layout = old->parentWidget()->layout();
        if (layout) {
            QLayoutItem* item = layout->replaceWidget(old, young);
            delete old;
            delete item;
            old = nullptr;
        }
        else {
            qDebug() << "replaceOld() 'old' argument had no layout";
            return;
        }
    }

    //template<typename T>
    //    requires(std::is_function_v<T>)
    //void forEachTile(Tile* (&arr)[8][8], std::function<T> func) {
    //    for (int x = 0; x < 8; x++) {
    //        for (int y = 0; y < 8; y++) {
    //            func(arr[x][y]);
    //        }
    //    }
    //}

#ifndef NDEBUG
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
#endif
}