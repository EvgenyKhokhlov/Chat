#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QValidator>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_cancel_clicked();

signals:
    void data_accept(QString host, int port);

private slots:
    void check_empty();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
