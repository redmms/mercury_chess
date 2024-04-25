#pragma once
#include <QScrollArea>

class QVBoxLayout;
class QWidget;
namespace mmd
{
    class Chat : public QScrollArea
    {
        Q_OBJECT

        QColor background_color;
        QWidget* message_box;
        QVBoxLayout* message_layout;
        QFont message_font;
        QFontMetrics message_metrics;
        int max_message_width;

    public:
        Chat(QWidget* parent_, QScrollArea* chat_area_, QColor background_color_ = Qt::white);

        void printMessage(QString name, bool own, QString text);
        void clearMessages();

    protected:
        void paintEvent(QPaintEvent* event) override;

    };
}