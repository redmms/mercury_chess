#ifndef CLOCK_H
#define CLOCK_H

#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QTime>

class ChessClock : public QObject
{
    Q_OBJECT

    QTimer* black_timer = new QTimer(this);
    QTimer* white_timer = new QTimer(this);
    QTimer* sec_counter = new QTimer(this);
    int white_remains;
    int black_remains;
    QLabel* black_label;
    QLabel* white_label;
    QTime zero_time = QTime(0, 0);
    int max_time;
    bool side;

public:
    ChessClock(QObject* parent, QLabel* opponent_label, QLabel* user_label, bool side_, int max_time_);
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
