#include "alarm.h"
#include "ui_alarm.h"
#include <QMessageBox>

Alarm::Alarm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Alarm)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    this->setAttribute(Qt::WA_TranslucentBackground);//背景透明
}

Alarm::~Alarm()
{
    delete ui;
}

void Alarm::on_calBtn_released()
{
    emit setAlarmTrue(true);
    this->hide();
    //this->deleteLater();
}


void Alarm::on_sureBtn_released()
{
    int H = ui->spinBoxH->value();
    int M = ui->spinBoxM->value();
    int S = ui->spinBoxS->value();

    //QMessageBox::information(this, "", QString("%1 %2 %3").arg(H).arg(M).arg(S));

    this->hide();
    emit setAlarmTime(H, M, S, true);


    //this->deleteLater();

}




