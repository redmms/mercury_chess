#ifndef RULES_DIALOG_H
#define RULES_DIALOG_H
#include <QDialog>

namespace Ui {
    class RulesDialog;
}

class RulesDialog : public QDialog
{
    Q_OBJECT
    
    Ui::RulesDialog* ui;

public:
    explicit RulesDialog(QWidget *parent = nullptr);
    ~RulesDialog() {
        delete ui;
    }
};

#endif // RULES_DIALOG_H