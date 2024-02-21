#ifndef RULES_DIALOG_H
#define RULES_DIALOG_H

#include <QDialog>

namespace Ui {
class RulesDialog;
}

class RulesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RulesDialog(QWidget *parent = nullptr);
    ~RulesDialog();

private:
    Ui::RulesDialog *ui;
};

#endif // RULES_DIALOG_H
