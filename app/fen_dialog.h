#pragma once
#include <QDialog>
#include <QScopedPointer>

namespace Ui 
{
    class FenDialog;
}
namespace mmd
{
    class FenDialog : public QDialog
    {
        Q_OBJECT
        QScopedPointer<Ui::FenDialog> ui;

    public:
        explicit FenDialog(QWidget* parent);
        ~FenDialog();

    signals:
        void newFen(QString fen);

    private slots:
        void on_skip_button_clicked();
        void on_save_button_clicked();
    };
}  // namespace mmd