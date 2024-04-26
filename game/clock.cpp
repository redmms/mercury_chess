#include "clock.h"
#include <QLabel>
#include <QTimer>
#include <QDebug>
using namespace std;

namespace mmd
{
    ChessClock::ChessClock(QObject* parent_, QLabel* opponent_label_, QLabel* user_label_,
        bool side_, int max_minutes_)
        : QObject(parent_),
        side(side_),
        black_label(side ? opponent_label_ : user_label_),
        white_label(side ? user_label_ : opponent_label_),
        max_time(max_minutes_ * 60000),
        black_remains(max_time),
        white_remains(max_time),
        black_timer(new QTimer(this)),
        white_timer(new QTimer(this)),
        sec_counter(new QTimer(this)),
        zero_time(QTime(0, 0))
    {
        for (auto timer : { black_timer, white_timer }) {
            timer->setInterval(max_time);
            connect(timer, &QTimer::timeout, this, &ChessClock::gameTimeout);
        }
        sec_counter->setTimerType(Qt::PreciseTimer);
        sec_counter->setInterval(1000);
        connect(sec_counter, &QTimer::timeout, this, &ChessClock::updateTimer);

        black_label->setText(zero_time.addMSecs(max_time).toString("mm:ss"));
        white_label->setText(zero_time.addMSecs(max_time).toString("mm:ss"));
    }

    void ChessClock::stopTimer()
    {
        if (!black_timer || !white_timer || !sec_counter)
            qDebug() << "ERROR: tried to stop deleted timer";
        else {
            black_timer->stop();
            white_timer->stop();
            sec_counter->stop();
        }
    }

    void ChessClock::updateTimer()
    {
        QLabel* label = black_timer->isActive() ? black_label : white_label;
        int rem = black_timer->isActive() ? black_timer->remainingTime() : white_timer->remainingTime();
        label->setText(zero_time.addMSecs(rem).toString("mm:ss"));
    }

    void ChessClock::gameTimeout()
    {
        stopTimer();
        QTimer* timer = (QTimer*)sender();
        if (side && timer == black_timer || !side && timer == white_timer)
            emit opponentOut();
        else if (side && timer == white_timer || !side && timer == black_timer)
            emit userOut();
    }

    void ChessClock::startTimer()
    {
        white_timer->start();
        sec_counter->start();
    }

    void ChessClock::switchTimer()
    {
        if (black_timer->isActive()) {
            black_remains = black_timer->remainingTime();
            black_timer->stop();
            white_timer->start(white_remains);
        }
        else {
            white_remains = white_timer->remainingTime();
            white_timer->stop();
            black_timer->start(black_remains);
        }
    }
}