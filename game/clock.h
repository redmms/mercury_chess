#pragma once
#include <QObject>
#include <QTime>

class QTimer;
class QLabel;
namespace mmd
{
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
        ChessClock(QObject* parent_, QLabel* opponent_label_, QLabel* user_label_, bool side_, int max_minutes_);

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
}