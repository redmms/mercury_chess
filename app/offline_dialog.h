#ifndef OFFLINE_DIALOG_H
#define OFFLINE_DIALOG_H
#include "../app/local_types.hpp"
#include "ui_offline_dialog.h"
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>

namespace Ui {
class OfflineDialog;
}

class OfflineDialog : public QDialog
{
    Q_OBJECT

    QPixmap default_pic;
    QPixmap opp_pic;
    QString opp_name;
    Ui::OfflineDialog *ui;

    bool readName();
public:
    explicit OfflineDialog(QWidget *parent = nullptr, QPixmap default_pic_ = {});
    ~OfflineDialog(){
        delete ui;
    }

signals:
    void newOppName(QString name);
    void newOppPic(QPixmap picture);

private slots:
    void on_choose_photo_button_clicked();
    void on_save_button_clicked();
    void on_dismiss_button_clicked();
};

#endif // OFFLINE_DIALOG_H
