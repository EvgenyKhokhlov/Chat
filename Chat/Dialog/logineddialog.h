#ifndef LOGINEDDIALOG_H
#define LOGINEDDIALOG_H

#include <QDialog>

namespace Ui {
class loginedDialog;
}

class loginedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginedDialog(QWidget *parent = nullptr);
    ~loginedDialog();

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
    void on_pushButton_cancel_clicked();
    void check_empty();

signals:
    void data_accept(QString login, QString password, bool isRegister);

private:
    Ui::loginedDialog *ui;
};

#endif // LOGINEDDIALOG_H
