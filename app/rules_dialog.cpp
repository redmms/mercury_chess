#include "rules_dialog.h"
#include "ui_rules_dialog.h"
using namespace std;

namespace mmd
{
    RulesDialog::RulesDialog(QWidget* parent_) :
        QDialog(parent_),
        ui(new Ui::RulesDialog)
    {
        ui->setupUi(this);
    }

    RulesDialog::~RulesDialog()
    {
        delete ui;
    }
}