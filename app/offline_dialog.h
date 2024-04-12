#include <QDialog>

namespace Ui {
    class OfflineDialog;
}

class OfflineDialog : public QDialog{
    Q_OBJECT

    Ui::OfflineDialog *ui;

    bool checkName();

public:
    explicit OfflineDialog(QWidget *parent_);
    ~OfflineDialog();

private slots:
    void on_choose_photo_button_clicked();
    void on_save_button_clicked();
    void on_skip_button_clicked();
};
