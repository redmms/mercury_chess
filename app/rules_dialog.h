#pragma once
#include <QDialog>
#include <QScopedPointer>

namespace Ui 
{
    class RulesDialog;
}
namespace mmd
{
    class RulesDialog : public QDialog 
    {
        Q_OBJECT
        QScopedPointer<Ui::RulesDialog> ui;

    public:
        explicit RulesDialog(QWidget* parent_);
        ~RulesDialog();
    };
}  // namespace mmd