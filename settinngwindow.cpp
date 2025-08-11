#include "settinngwindow.h"
#include "ui_settinngwindow.h"
#include <QMessageBox>

SettinngWindow::SettinngWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettinngWindow)
{
    ui->setupUi(this);

    this->setFixedSize(650, 500);


    ui->self_model_lineEidt->setReadOnly(true);
    ui->self_key_lineEidt->setReadOnly(true);
    ui->self_url_lineEidt->setReadOnly(true);
}

SettinngWindow::~SettinngWindow()
{
    delete ui;
}

void SettinngWindow::on_setLabel(int times, QString model, QString url, QString key, bool selfseted)
{
    ui->label_12->setText(QString("%1").arg(times));

    //QMessageBox::information(this, "", QString("%1").arg(selfseted));
    if(selfseted){
        ui->self_model_lineEidt->setText(model);
        ui->self_url_lineEidt->setText(url);
        ui->self_key_lineEidt->setText(key);
    }
}

void SettinngWindow::closeEvent(QCloseEvent *event)
{
    // 隐藏窗口，而非关闭
    this->hide();
    // 忽略默认的关闭事件（否则窗口会被销毁）
    event->ignore();
}

void SettinngWindow::on_pushButton_released()
{
    ui->self_model_lineEidt->setReadOnly(false);
    ui->self_key_lineEidt->setReadOnly(false);
    ui->self_url_lineEidt->setReadOnly(false);

    setStyleSheet(
        "QLineEdit {"
        "    border-radius: 8px;"
        "    border: 1px solid #b3d1ff;"
        "    background-color: #e6f0ff;"
        "    padding: 2px 8px;"
        "    font-family: \"楷体\", \"KaiTi\", serif;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #B24DFF;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #7eb3ff;"
        "    background-color: #cce0ff;"
        "}"
        );
}


void SettinngWindow::on_pushButton_3_released()
{
    ui->self_model_lineEidt->setReadOnly(true);
    ui->self_key_lineEidt->setReadOnly(true);
    ui->self_url_lineEidt->setReadOnly(true);

    QUrl url = QUrl(QString(ui->self_url_lineEidt->text()));
    QString model = ui->self_model_lineEidt->text();
    QString key = ui->self_key_lineEidt->text();
    int t = ui->slider->value();
    double temp = static_cast<double>(t) / 10;
    //QMessageBox::information(this, "", "O而K之!");

    emit signal_setSelf_model(url, model, key, temp);

    setStyleSheet("");

}







