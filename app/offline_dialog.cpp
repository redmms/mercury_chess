#include "offline_dialog.h"
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
using namespace std;

OfflineDialog::OfflineDialog(QWidget *parent, QPixmap default_pic_) :
    QDialog(parent),
    default_pic(default_pic_),
    opp_pic(default_pic_),
    ui(new Ui::OfflineDialog)
{
    ui->setupUi(this);
    ui->friend_avatar->setPixmap(default_pic);
    //setStyleSheet("background-image: url(:/images/background);");

    auto mask_size = ui->friend_avatar->width();
    QPixmap  pix(mask_size, mask_size); // initialize a mask for avatar's rounded corners
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setBrush(Qt::color1); //Qt::black
    painter.drawRoundedRect(0, 0, mask_size, mask_size, 14, 14);
    QBitmap pic_mask = pix.createMaskFromColor(Qt::transparent);

    ui->friend_avatar->setMask(pic_mask);
    setWindowTitle("Choose your opponent's name");
}

bool OfflineDialog::readName()
{
    QString new_name = ui->friend_edit->text();
    if (new_name.isEmpty()) {
        showBox("Embarrasing!",
            "This nickname is too small. It can't be empty.",
            QMessageBox::Warning);
        return false;
    }
    else if (new_name.size() > 12) {
        showBox("So huge!",
            "This nickname is too long. Maximum length is 12" /*+ QString::number(max_nick)*/,
            QMessageBox::Warning);
        return false;
    }
    else {
        opp_name = new_name;
        return true;
    }
}


void OfflineDialog::on_choose_photo_button_clicked()
{
    QString avatar_address = QFileDialog::getOpenFileName(this, "Open File",
        QString("Choose a photo for opponent's avatar. Avatar picture will be scaled to 100x100 pixel image."),
        tr("Images (*.png *.jpg *.jpeg *.pgm)"));
    QSize user_size = {100, 100};
    if (!avatar_address.isEmpty()){
        opp_pic = QPixmap(avatar_address).scaled(user_size);
        ui->friend_avatar->setPixmap(opp_pic);
    }
}


void OfflineDialog::on_save_button_clicked()
{
    bool name_change_successful = readName();
    if (name_change_successful){
        emit newOppPic(opp_pic);
        emit newOppName(opp_name);
        accept();
    }
    //this->~OfflineDialog();
}

void OfflineDialog::on_dismiss_button_clicked()
{
    reject();
    //this->~OfflineDialog();
}

