#include "rules_dialog.h"
#include "ui_rules_dialog.h"

RulesDialog::RulesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RulesDialog)
{
    ui->setupUi(this);
}

RulesDialog::~RulesDialog()
{
    delete ui;
}