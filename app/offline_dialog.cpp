#include "offline_dialog.h"
#include "ui_offline_dialog.h"
#include "../app/local_types.h"
#include <QFileDialog>
#include <QBitmap>
using namespace std;

namespace mmd
{
    OfflineDialog::OfflineDialog(QWidget* parent_) :
        QDialog(parent_),
        ui(new Ui::OfflineDialog)
    {
        ui->setupUi(this);
        ui->friend_avatar->setPixmap(getPic(def_pic_e));
        ui->friend_avatar->setMask(getBMap(pic_mask_e));
    }

    OfflineDialog::~OfflineDialog() = default;

    bool OfflineDialog::checkName()
    {
        QString new_name = ui->friend_edit->text();
        int max_nick = settings[max_nick_e].toInt();
        if (new_name.isEmpty()) {
            showBox("Embarrasing!",
                "This nickname is too small. It can't be empty.",
                QMessageBox::Warning);
            return false;
        }
        else if (new_name.size() > max_nick) {
            showBox("So huge!",
                "This nickname is too long. Maximum length is " + QString::number(max_nick),
                QMessageBox::Warning);
            return false;
        }
        else {
            return true;
        }
    }

    void OfflineDialog::on_choose_photo_button_clicked()
    {
        int width = settings[pic_w_e].toInt();
        int height = settings[pic_h_e].toInt();
        QString w = QString::number(width);
        QString h = QString::number(height);
        QString avatar_address = QFileDialog::getOpenFileName(this, "Open File",
            QString("Choose a photo for opponent's avatar. Avatar picture will be scaled to " + w + "x" + h + " pixel image."),
            tr("Images (*.png *.jpg *.jpeg *.pgm)"));
        if (!avatar_address.isEmpty()) {
            ui->friend_avatar->setPixmap(QPixmap(avatar_address).scaled(width, height));
        }
    }

    void OfflineDialog::on_save_button_clicked()
    {
        if (checkName()) {
            setPic(opp_pic_e, *ui->friend_avatar->pixmap());
            settings[opp_name_e].setValue(ui->friend_edit->text());
            accept();
        }
    }

    void OfflineDialog::on_skip_button_clicked()
    {
        reject();
    }
}  // namespace mmd