#include <QDialog>

namespace Ui {
    class FenDialog;
}
class FenDialog : public QDialog
{
    Q_OBJECT
    
    Ui::FenDialog* ui;

public:
    FenDialog(QWidget *parent);
    ~FenDialog();

signals:
    void newFen(QString fen);

private slots:
    void on_skip_button_clicked();
    void on_save_button_clicked();
};