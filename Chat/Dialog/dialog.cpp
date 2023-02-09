#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    ui->lineEdit_ip->setInputMask("000.000.000.000");
    ui->lineEdit_port->setValidator(new QIntValidator(0, 65535));

    ui->pushButton_connect->setEnabled(false);

    connect(ui->lineEdit_ip, &QLineEdit::textEdited, this, &Dialog::check_empty);
    connect(ui->lineEdit_port, &QLineEdit::textEdited, this, &Dialog::check_empty);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_connect_clicked()
{
    emit data_accept(ui->lineEdit_ip->text(), ui->lineEdit_port->text().toInt());
    this->accept();
}

void Dialog::on_pushButton_cancel_clicked()
{
    this->reject();
}

void Dialog::check_empty()
{
    if (!ui->lineEdit_ip->text().isEmpty() && !ui->lineEdit_port->text().isEmpty())
        ui->pushButton_connect->setEnabled(true);

    if (ui->lineEdit_ip->text().isEmpty() || ui->lineEdit_port->text().isEmpty())
        ui->pushButton_connect->setEnabled(false);
}
