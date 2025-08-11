#ifndef SETTINNGWINDOW_H
#define SETTINNGWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QUrl>

namespace Ui {
class SettinngWindow;
}

class SettinngWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettinngWindow(QWidget *parent = nullptr);
    ~SettinngWindow();

    void on_setLabel(int times, QString model, QString url, QString key, bool selfseted);

    void closeEvent(QCloseEvent *event) override;

signals:
    void signal_setSelf_model(QUrl, QString, QString, double);

private slots:
    void on_pushButton_released();

    void on_pushButton_3_released();

private:
    Ui::SettinngWindow *ui;

    int lefttimes;
};

#endif // SETTINNGWINDOW_H
