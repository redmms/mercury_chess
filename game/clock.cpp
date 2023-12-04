#include "clock.h"

ChessClock::ChessClock(QObject* parent, QLabel* opponent_label, QLabel* user_label,
                       bool side_, int max_time_)
    : QObject(parent),
      side(side),
      black_label(side_ ? opponent_label : user_label),
      white_label(side_ ? user_label : opponent_label),
      max_time(max_time_),
      white_remains(max_time_),
      black_remains(max_time_)
{
    for (auto timer: {black_timer, white_timer}){
        //timer->setTimerType(Qt::PreciseTimer); // this line will really slow down the game
        timer->setInterval(max_time);
        connect(timer, &QTimer::timeout, this, &ChessClock::gameTimeout);
    }
    sec_counter->setTimerType(Qt::PreciseTimer);
    sec_counter->setInterval(1000);
    connect(sec_counter, &QTimer::timeout, this, &ChessClock::updateTimer);

    black_label->setText(zero_time.addMSecs(max_time).toString("mm:ss"));
    white_label->setText(zero_time.addMSecs(max_time).toString("mm:ss"));
}

ChessClock::~ChessClock()
{
    black_timer->~QTimer();
    white_timer->~QTimer();
    sec_counter->~QTimer();
}

void ChessClock::stopTimer()
{
    black_timer->stop();
    white_timer->stop();
    sec_counter->stop();
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
    QTimer* timer = (QTimer*) sender();
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
    if (black_timer->isActive()){
        black_remains = black_timer->remainingTime();
        black_timer->stop();
        white_timer->start(white_remains);
    }
    else{
        white_remains = white_timer->remainingTime();
        white_timer->stop();
        black_timer->start(black_remains);
    }
}
