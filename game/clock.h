#pragma once
#include <QObject>
#include <QTime>

class QTimer;
class QLabel;
class ChessClock : public QObject
{
    Q_OBJECT

    bool side;
    QLabel* black_label;
    QLabel* white_label;
    int max_time;
    int black_remains;
    int white_remains;
    QTimer* black_timer;
    QTimer* white_timer;
    QTimer* sec_counter;
    QTime zero_time;

public:
    ChessClock(QObject* parent, QLabel* opponent_label, QLabel* user_label, bool side_, int max_minutes);

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