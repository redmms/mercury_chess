#pragma once
#include <QDialog>
#include <QScopedPointer>

namespace Ui 
{
    class OfflineDialog;
}
namespace mmd
{
    class OfflineDialog : public QDialog 
    {
        Q_OBJECT
        QScopedPointer<Ui::OfflineDialog> ui;

        bool checkName();

    public:
        explicit OfflineDialog(QWidget* parent_);
        ~OfflineDialog();

    private slots:
        void on_choose_photo_button_clicked();
        void on_save_button_clicked();
        void on_skip_button_clicked();
    };
}  // namespace mmd