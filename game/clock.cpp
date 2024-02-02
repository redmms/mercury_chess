#include "clock.h"
#include <QtCore/qobjectdefs.h>
#include <QLabel>
#include "qnamespace.h"
#include <QDebug>
#include "local_types.h"

ChessClock::ChessClock(QObject* parent = 0, QLabel* opponent_label = 0, QLabel* user_label = 0,
    bool side_ = false, int max_minutes = 0)
    : QObject(parent),
    side(side_),
	black_label(side_ ? opponent_label : user_label),
	white_label(side_ ? user_label : opponent_label),
	max_time(max_minutes * 60000),
	black_remains(max_time),
    white_remains(max_time),
    black_timer(new QTimer(this)),
    white_timer(new QTimer(this)),
    sec_counter(new QTimer(this)),
    zero_time(QTime(0, 0))
{
    for (auto timer : { black_timer.data(), white_timer.data() }) {
		//timer->setTimerType(Qt::PreciseTimer); // this line will really slow down the game
		timer->setInterval(max_time);
		connect(timer, &QTimer::timeout, this, &ChessClock::gameTimeout);
	}
	sec_counter->setTimerType(Qt::PreciseTimer);
	sec_counter->setInterval(1000);
    connect(sec_counter.data(), &QTimer::timeout, this, &ChessClock::updateTimer);

	black_label->setText(zero_time.addMSecs(max_time).toString("mm:ss"));
	white_label->setText(zero_time.addMSecs(max_time).toString("mm:ss"));
}

void ChessClock::stopTimer()
{
    if (black_timer)
        black_timer->stop();
    else
        qDebug() << "ERROR: tried to stop deleted timer";

    if (white_timer)
        white_timer->stop();
    else
        qDebug() << "ERROR: tried to stop deleted timer";

    if (sec_counter)
        sec_counter->stop();
    else
        qDebug() << "ERROR: tried to stop deleted timer";
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
    if (side && timer == black_timer.data() || !side && timer == white_timer.data())
		emit opponentOut();
    else if (side && timer == white_timer.data() || !side && timer == black_timer.data())
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
