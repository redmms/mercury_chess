#pragma once
#include "offline_dialog.h"
#include "ui_offline_dialog.h"
#include "../app/local_types.h"
#include <QPainter>
#include <QBitmap>
#include <QFileDialog>
using namespace std;

OfflineDialog::OfflineDialog(QWidget *parent_) :
    QDialog(parent_),
    ui(new Ui::OfflineDialog)
{
    ui->setupUi(this);
    ui->friend_avatar->setPixmap(getPic("def_pic"));
    ui->friend_avatar->setMask(getBMap("pic_mask"));
    setWindowTitle("Choose your opponent's name");
}

OfflineDialog::~OfflineDialog() {
    delete ui;
}

bool OfflineDialog::checkName()
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
        return true;
    }
}

void OfflineDialog::on_choose_photo_button_clicked()
{
    QString avatar_address = QFileDialog::getOpenFileName(this, "Open File",
        QString("Choose a photo for opponent's avatar. Avatar picture will be scaled to 100x100 pixel image."),
        tr("Images (*.png *.jpg *.jpeg *.pgm)"));
    if (!avatar_address.isEmpty()){
        ui->friend_avatar->setPixmap(QPixmap(avatar_address).scaled(100, 100));
    }
}

void OfflineDialog::on_save_button_clicked()
{
    if (checkName()){
        setPic("opp_pic", *ui->friend_avatar->pixmap());
        settings["opp_name"].setValue(ui->friend_edit->text());
        accept();
    } 
}

void OfflineDialog::on_dismiss_button_clicked()
{
    reject();
}