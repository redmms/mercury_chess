#include "chat.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QPainter>
using namespace std;

Chat::Chat(QWidget* parent_, QScrollArea* chat_area_, QColor background_color_) :
    QScrollArea(parent_),
    background_color(background_color_),
    message_layout(new QVBoxLayout()),
    message_box(new QWidget(this)),
    message_font{ "Segoe Print", 12 },
    message_metrics{ message_font },
    max_message_width{}
{
    // prepare scroll_area before making a chat
    setStyleSheet("QAbstractScrollArea{background: transparent; border: none;}");
    setGeometry(chat_area_->geometry());
    setSizePolicy(chat_area_->sizePolicy());
    setMinimumSize(chat_area_->minimumSize());
    setMaximumSize(chat_area_->maximumSize());
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chat_area_->parentWidget()->layout()->replaceWidget(chat_area_, this);
    chat_area_->~QScrollArea();

    //chat itself
    max_message_width = minimumWidth() - 20;
    message_layout->setContentsMargins(10, 5, 10, 5);
    message_box->setLayout(message_layout);
    message_box->resize(width() - 28, 0);
    message_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    message_box->setStyleSheet("background-color: transparent;"); //#1B1C1F
    setWidget(message_box);
    setWidgetResizable(true);

}

void Chat::printMessage(QString name, bool own, QString text)
{
    if (text.isEmpty())
        return;

    QChar ch;
    for (int cur_len = 0, pos = 0; pos < text.size(); pos++) {
        ch = text[pos];
        if (ch == ' ' || ch == '\n')
            cur_len = 0;
        else {
            cur_len += message_metrics.size(0, ch).width();
            if (cur_len > (max_message_width - 10)) {
                text.insert(pos, "\n");
                cur_len = 0;
            }
        }
    }

    QLabel* message(new QLabel(this));
    if (own)
        message->setStyleSheet("background-color: rgb(0,179,60); border-radius: 14;");
    else
        message->setStyleSheet("background-color: rgb(0,128,21); border-radius: 14;");
    message->setIndent(5);
    message->setMargin(5);
    message->setMaximumWidth(max_message_width);
    message->setFont(message_font);
    message->setWordWrap(true);
    message->setText(name + "\n" + text);
    message->adjustSize();
    message->setMinimumSize(message->size());
    message->setMaximumSize(message->size());
    if (own)
        message_layout->addWidget(message, 0, Qt::AlignTop | Qt::AlignRight);
    else
        message_layout->addWidget(message, 0, Qt::AlignTop | Qt::AlignLeft);
    message_box->resize(message_box->width(), message_box->height() + message->height() + 10);
    // 10 is layout margin here, for shortness

    QTimer::singleShot(100, [&]() {
        auto scroller = verticalScrollBar();
        scroller->setValue(scroller->maximum());
        });
}

void Chat::clearMessages()
{
    for (QLayoutItem* child; (child = message_layout->takeAt(0)) != nullptr; child->widget()->~QWidget()) {}
    message_box->resize(width(), 0);
}

void Chat::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(viewport());
    int radius = 14;
    painter.setBrush(background_color); // QColor(0, 102, 51)
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(viewport()->rect(), radius, radius);
}
