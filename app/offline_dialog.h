#pragma once
#include <QDialog>

//FIX: could be simplified, using settings variable
namespace Ui {
    class OfflineDialog;
}

class OfflineDialog : public QDialog{
    Q_OBJECT

    QPixmap default_pic;
    QPixmap opp_pic;
    QString opp_name;
    Ui::OfflineDialog *ui;

    bool readName();
public:
    explicit OfflineDialog(QWidget *parent, QPixmap default_pic_);
    ~OfflineDialog();

signals:
    void newOppName(QString name);
    void newOppPic(QPixmap picture);

private slots:
    void on_choose_photo_button_clicked();
    void on_save_button_clicked();
    void on_dismiss_button_clicked();
};