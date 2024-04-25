#include "fen_dialog.h"
#include "ui_fen_dialog.h"

namespace mmd
{
    FenDialog::FenDialog(QWidget* parent) :
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
        QString fen = ui->fen_edit->text();
        if (fen.isEmpty())
            fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
        emit newFen(fen);
        accept();
    }
}