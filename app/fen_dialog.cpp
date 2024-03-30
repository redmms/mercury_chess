#include "fen_dialog.h"
#include "ui_fen_dialog.h"

FenDialog::FenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FenDialog)
{
    ui->setupUi(this);
}

FenDialog::~FenDialog()
{
    delete ui;
}

void FenDialog::on_skip_button_clicked()
{
    reject();
}

void FenDialog::on_save_button_clicked()
{
    emit newFen(ui->fen_edit->text());
    accept();
}

