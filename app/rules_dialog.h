#pragma once
#include <QDialog>

namespace Ui {
    class RulesDialog;
}

namespace mmd
{
    class RulesDialog : public QDialog 
    {
        Q_OBJECT

        Ui::RulesDialog* ui;

    public:
        explicit RulesDialog(QWidget* parent_);
        ~RulesDialog();
    };
}