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

public:
    explicit ChessClock(QObject* parent, QLabel* _black_label, QLabel* _white_label, int _max_time);

    void stopTimer();

signals:
    void whiteOut();
    void blackOut();

private slots:
    void updateTimer();
    void gameTimeout();

public slots:
    void startTimer();
    void switchTimer();
};

#endif // CLOCK_H
