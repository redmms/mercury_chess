#include "clock.h"

ChessClock::ChessClock(QObject* parent, QLabel* _black_label, QLabel* _white_label, int _max_time)
    : QObject{parent},
      black_label(_black_label),
      white_label(_white_label),
      max_time(_max_time),
      white_remains(_max_time),
      black_remains(_max_time)
{
    for (auto timer: {black_timer, white_timer}){
        timer->setTimerType(Qt::PreciseTimer);
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
    if (timer == black_timer)
        emit blackOut();
    else if (timer == white_timer)
        emit whiteOut();
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
