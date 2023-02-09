#include "logineddialog.h"
#include "ui_logineddialog.h"

loginedDialog::loginedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginedDialog)
{
    ui->setupUi(this);

    connect(ui->lineEdit_login, &QLineEdit::textEdited, this, &loginedDialog::check_empty);
    connect(ui->lineEdit_password, &QLineEdit::textEdited, this, &loginedDialog::check_empty);

    ui->pushButton_login->setEnabled(false);
    ui->pushButton_register->setEnabled(false);
}

loginedDialog::~loginedDialog()
{
    delete ui;
}

void loginedDialog::on_pushButton_login_clicked()
{
    emit data_accept(ui->lineEdit_login->text(), ui->lineEdit_password->text(), false);
    this->accept();
}

void loginedDialog::on_pushButton_register_clicked()
{
    emit data_accept(ui->lineEdit_login->text(), ui->lineEdit_password->text(), true);
    this->accept();
}

void loginedDialog::on_pushButton_cancel_clicked()
{
    this->reject();
}

void loginedDialog::check_empty()
{
    if (!ui->lineEdit_login->text().isEmpty() && !ui->lineEdit_login->text().isEmpty()){
        ui->pushButton_register->setEnabled(true);
        ui->pushButton_login->setEnabled(true);
    }

    if (ui->lineEdit_password->text().isEmpty() || ui->lineEdit_password->text().isEmpty()){
        ui->pushButton_register->setEnabled(false);
        ui->pushButton_login->setEnabled(false);
    }

}
