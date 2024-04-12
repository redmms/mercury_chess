#include <QDialog>

namespace Ui {
    class RulesDialog;
}

class RulesDialog : public QDialog{
    Q_OBJECT
    
    Ui::RulesDialog* ui;

public:
    explicit RulesDialog(QWidget *parent);
    ~RulesDialog();
};
