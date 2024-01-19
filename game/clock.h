#ifndef CLOCK_H
#define CLOCK_H

#include "qdatetime.h"
#include "qobject.h"
#include <QPointer>
#include <QScopedPointer>

class QLabel;
class QTimer;


class ChessClock : public QObject
{
	Q_OBJECT

	bool side;
    QPointer<QLabel> black_label;
    QPointer<QLabel> white_label;
	int max_time;
	int black_remains;
	int white_remains;
    QScopedPointer<QTimer> black_timer;
    QScopedPointer<QTimer> white_timer;
    QScopedPointer<QTimer> sec_counter;
	QTime zero_time;

public:
	ChessClock(QObject* parent, QLabel* opponent_label, QLabel* user_label, bool side_, int max_minutes);
	~ChessClock();

	void stopTimer();

signals:
	void userOut();
	void opponentOut();

private slots:
	void updateTimer();
	void gameTimeout();

public slots:
	void startTimer();
	void switchTimer();
};

#endif // CLOCK_H
