#ifndef ALARM_H
#define ALARM_H

#include <QWidget>

namespace Ui {
class Alarm;
}

class Alarm : public QWidget
{
    Q_OBJECT

public:
    explicit Alarm(QWidget *parent = nullptr);
    ~Alarm();

signals:
    void setAlarmTime(int, int, int, bool);
    void setAlarmTrue(bool);

private slots:
    void on_calBtn_released();

    void on_sureBtn_released();

private:
    Ui::Alarm *ui;

    int timeH;
    int timeM;
    int timeS;
};

#endif // ALARM_H
