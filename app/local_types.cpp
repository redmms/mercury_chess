#include "local_types.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QObject>
#include <QIcon>
#include <QBuffer>
#include <QBitmap>
#include <QString>
#include <QBitmap>
#include <QVariant>
#include <string>
using namespace std;

namespace mmd
{
    void showBox(QString header,
        QString text,
        QMessageBox::Icon icon_type)
    {
        QMessageBox msg_box;
        msg_box.setWindowTitle(header);
        msg_box.setWindowIcon(QIcon(":/images/app_icon"));
        msg_box.setText(text);
        msg_box.setIcon(icon_type);
        msg_box.addButton("Ok", QMessageBox::AcceptRole);
        msg_box.addButton("Donate", QMessageBox::RejectRole);
        QObject::connect(&msg_box, &QMessageBox::rejected, [&]() {
            QDesktopServices::openUrl(QUrl("https://ko-fi.com/mmd18", QUrl::TolerantMode));
            });
        msg_box.exec();
    }

    void setPic(setnum par, const QPixmap& pic) 
    {
        QByteArray arr;
        QBuffer buffer(&arr);
        buffer.open(QIODevice::WriteOnly);
        pic.save(&buffer, "PNG");
        settings[par].setValue(arr);
    }

    QPixmap getPic(setnum par) 
    {
        QByteArray arr = settings[par].toByteArray();
        QPixmap pic;
        pic.loadFromData(arr, "PNG");
        return pic;
    }

    QString coordToString(scoord coord)
    {
        return QString(char('a' + coord.x)) + QString::number(coord.y + 1);
    }

    scoord stringToCoord(QString str)
    {
        if (str.size() < 2) {
            throw invalid_argument("In local_types.h, in stringToCoord(QString str)");
        }
        string s = str.toStdString();
        return { s[0] - 'a', s[1] - '0' - 1 };
    }

    const map<char, promnum> promo_by_char
    {
        {'N', to_knight},
        {'B', to_bishop},
        {'R', to_castle},
        {'Q', to_queen},
        {'e', no_promotion}
    };

    const map<promnum, char> char_by_promo
    {
        {to_knight, 'N'},
        {to_bishop, 'B'},
        {to_castle, 'R'},
        {to_queen, 'Q'},
        {no_promotion, 'e'}
    };

    void setBMap(setnum par, const QBitmap& bmap)
    {
        settings[par] = QVariant(bmap);
    }

    QBitmap getBMap(setnum par)
    {
        return qvariant_cast<QBitmap>(settings[par]);
    }

    map<setnum, QVariant> settings;
 
#ifndef NDEBUG
    scoord a1 = stringToCoord("a1");
    scoord a2 = stringToCoord("a2");
    scoord a3 = stringToCoord("a3");
    scoord a4 = stringToCoord("a4");
    scoord a5 = stringToCoord("a5");
    scoord a6 = stringToCoord("a6");
    scoord a7 = stringToCoord("a7");
    scoord a8 = stringToCoord("a8");
    scoord b1 = stringToCoord("b1");
    scoord b2 = stringToCoord("b2");
    scoord b3 = stringToCoord("b3");
    scoord b4 = stringToCoord("b4");
    scoord b5 = stringToCoord("b5");
    scoord b6 = stringToCoord("b6");
    scoord b7 = stringToCoord("b7");
    scoord b8 = stringToCoord("b8");
    scoord c1 = stringToCoord("c1");
    scoord c2 = stringToCoord("c2");
    scoord c3 = stringToCoord("c3");
    scoord c4 = stringToCoord("c4");
    scoord c5 = stringToCoord("c5");
    scoord c6 = stringToCoord("c6");
    scoord c7 = stringToCoord("c7");
    scoord c8 = stringToCoord("c8");
    scoord d1 = stringToCoord("d1");
    scoord d2 = stringToCoord("d2");
    scoord d3 = stringToCoord("d3");
    scoord d4 = stringToCoord("d4");
    scoord d5 = stringToCoord("d5");
    scoord d6 = stringToCoord("d6");
    scoord d7 = stringToCoord("d7");
    scoord d8 = stringToCoord("d8");
    scoord e1 = stringToCoord("e1");
    scoord e2 = stringToCoord("e2");
    scoord e3 = stringToCoord("e3");
    scoord e4 = stringToCoord("e4");
    scoord e5 = stringToCoord("e5");
    scoord e6 = stringToCoord("e6");
    scoord e7 = stringToCoord("e7");
    scoord e8 = stringToCoord("e8");
    scoord f1 = stringToCoord("f1");
    scoord f2 = stringToCoord("f2");
    scoord f3 = stringToCoord("f3");
    scoord f4 = stringToCoord("f4");
    scoord f5 = stringToCoord("f5");
    scoord f6 = stringToCoord("f6");
    scoord f7 = stringToCoord("f7");
    scoord f8 = stringToCoord("f8");
    scoord g1 = stringToCoord("g1");
    scoord g2 = stringToCoord("g2");
    scoord g3 = stringToCoord("g3");
    scoord g4 = stringToCoord("g4");
    scoord g5 = stringToCoord("g5");
    scoord g6 = stringToCoord("g6");
    scoord g7 = stringToCoord("g7");
    scoord g8 = stringToCoord("g8");
    scoord h1 = stringToCoord("h1");
    scoord h2 = stringToCoord("h2");
    scoord h3 = stringToCoord("h3");
    scoord h4 = stringToCoord("h4");
    scoord h5 = stringToCoord("h5");
    scoord h6 = stringToCoord("h6");
    scoord h7 = stringToCoord("h7");
    scoord h8 = stringToCoord("h8");
#endif
}