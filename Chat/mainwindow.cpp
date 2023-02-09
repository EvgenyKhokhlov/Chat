#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    messagesStore = new MessageStore;

    this->setTrayIconActions();
    this->showTrayIcon();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init_server()
{
    tcpServer = new QTcpServer(this);

    QHostAddress ipAddress;

    if (!ipAddress.setAddress(host)) {
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        int count = ipAddressesList.count();

        for (int i = 0; i < count; i++) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()){
                ipAddress = ipAddressesList.at(i);
                break;
            }
        }
    }

    if (ipAddress.isNull()) ipAddress = QHostAddress(QHostAddress::LocalHost);

    if (!tcpServer->listen(ipAddress, port))
    {
        ui->textBrowser->append("Server not started: " +tcpServer->errorString());
        return false;
    }

    ui->textBrowser->append("TCP server started at " + ipAddress.toString() + ":"
                                                     + QString::number(tcpServer->serverPort()));

    log_sql = new sql;

    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::client_connected_to_server);

    return true;
}

bool MainWindow::init_client()
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::client_connected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::client_ready_read);

    socket->connectToHost(host, port);

    if (!socket->waitForConnected(1000)) {
        ui->textBrowser->append("Error " + socket->errorString());
        return  false;
    }
    ui->textBrowser->append("Connection established");
    return  true;
}

void MainWindow::client_ready_read()
{
    QString temp;
    temp = socket->readAll();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(temp.toUtf8(), &jsonError);

    if (QJsonParseError::NoError != jsonError.error) {
        if(isClientLogined) ui->textBrowser->append(temp);
    }else {
        QJsonObject obj = doc.object();
        if (obj["Text"].toString() != nullptr) ui->textBrowser->append(obj["Text"].toString());

        if (obj["isLogined"] == true) {
            chat_active();
            isClientLogined = true;
            ui->textBrowser->append("Logined to server.");
        }

        QJsonArray arr = obj["SavedMessages"].toArray();

        for (int i = 0; i < arr.count(); i++) {
            QJsonObject jsonMessages = arr[i].toObject();
            ui->textBrowser->append(jsonMessages["Message"].toString());
        }
    }
}

void MainWindow::client_connected(){ }

void MainWindow::client_connected_to_server()
{
    Client *c = new Client;
    c->socket = tcpServer->nextPendingConnection();
    clients.push_back(c);

    connect(c->socket, &QAbstractSocket::disconnected, c->socket, &QObject::deleteLater);
    connect(c->socket, &QTcpSocket::readyRead, this, &MainWindow::get_message);
}

void MainWindow::get_message()
{
    QObject *obj = sender();
    QString temp;
    QString message;
    bool isJsonMessage = false;

    for ( int i = 0; i < clients.size(); i++) {
        if(obj == clients[i]->socket){
            temp = clients[i]->socket->readAll();

            QJsonParseError jsonError;
            QJsonDocument doc = QJsonDocument::fromJson(temp.toUtf8(), &jsonError);

            if (QJsonParseError::NoError != jsonError.error) {
                message = QTime::currentTime().toString("hh:mm") + " " + clients[i]->name + ": " + temp + "\n";
            }else{
                QJsonObject obj = doc.object();
                bool isRegister = obj["isRegister"].toBool();
                if(isRegister){
                    if (!log_sql->set_user(obj["Login"].toString(), obj["Password"].toString())) {
                        clients[i]->socket->write("User: " + obj["Login"].toString().toUtf8() + " already registered.");
                    }else {
                        clients[i]->name = obj["Login"].toString();

                        message = send_message_on_client_logined(clients[i], isRegister);

                        ui->textBrowser->append("User: " + clients[i]->name + " registered.");
                    }
                }else if(!isRegister && log_sql->get_user(obj["Login"].toString(),
                                                                         obj["Password"].toString())){
                    bool check = true;
                    for ( int j = 0; j < clients.size(); j++) {
                        if (clients[j]->name == obj["Login"].toString() || server_name == obj["Login"].toString()) {
                            clients[j]->socket->write("User: " + obj["Login"].toString().toUtf8() + " already logined.");
                            check = false;
                            break;
                        }
                    }
                    if (check) {
                        clients[i]->name =obj["Login"].toString();

                        message = send_message_on_client_logined(clients[i], isRegister);
                    }
                }else{
                    clients[i]->socket->write("Wrong login or password");
                }
            isJsonMessage = true;
            }
            ui->textBrowser->append(message);
            messagesStore->saveMessage(message);           
            break;
        }
    }

    for (int i = 0; i < clients.size(); i++) {
        if(clients[i]->logined && obj != clients[i]->socket) {
            clients[i]->socket->write(message.toUtf8());
        }else if(!isJsonMessage) {
            clients[i]->socket->write(message.toUtf8());
        }
    }
}

QString MainWindow::send_message_on_client_logined(Client* client, bool isRegister)
{
    QJsonObject jsonObject;
    QJsonArray jsonArray;
    client->logined = true;

    for (int i = 0; i < messagesStore->getMessages()->size(); i++) {
        QJsonObject jsonArrayObject;
        jsonArrayObject["Message"] = messagesStore->getMessages()->at(i);
        jsonArray.push_back(jsonArrayObject);
    }

    jsonObject["SavedMessages"] = jsonArray;
    jsonObject["isLogined"] = client->logined;
    if(isRegister) jsonObject["Text"] = "You successfully registered as: " + client->name;

    client->socket->write(QJsonDocument(jsonObject).toJson());

    QString message = "User: " + client->name + " connected to server at: "
                       + QTime::currentTime().toString("hh:mm") + "\n";

    return message;
}

void MainWindow::on_pushButton_init_server_clicked()
{
    isServer = true;
    open_dialog();
}

void MainWindow::on_pushButton_connect_client_clicked()
{
    open_dialog();
}

void MainWindow::open_dialog()
{
    Dialog *d = new Dialog;
    connect(d, &Dialog::data_accept, this, &MainWindow::get_result_from_dialog);
    d->exec();
}

void MainWindow::get_result_from_dialog(QString dialogHost, int dialogPort)
{
    port = dialogPort;
    host = dialogHost;

    if (isServer) {
        if(init_server()) ui->pushButton_login->setEnabled(true);
    }else{
        if(init_client()) ui->pushButton_login->setEnabled(true);
    }
}

void MainWindow::get_result_from_logined_dialog(QString login, QString password, bool isRegister)
{
    if (isServer) {
        if (isRegister) {
            if (!log_sql->set_user(login, password)) {
                ui->textBrowser->append("User: " + login + " already registered.");
            }else {
                server_name = login;
                ui->textBrowser->append("New user: " + login + " registered.");
                chat_active();
            }
        }else{
            if (log_sql->get_user(login, password)) {
                bool check = true;
                if (server_name == login) {
                    ui->textBrowser->append("User: " + login + " alraedy logined.");
                    check = false;
                }
                for ( int i = 0; i < clients.size(); i++) {
                    if (clients[i]->name == login) {
                        ui->textBrowser->append("User: " + login + " alraedy logined.");
                        check = false;
                        break;
                    }
                }
                if (check) {
                    server_name = login;
                    ui->textBrowser->append("User: " + login + " logined.");
                    chat_active();
                }
            }else {
                ui->textBrowser->append("Wrong login or password.");
            }
        }
    }else{
        QJsonObject jsonObject;

        jsonObject["Login"] = login;
        jsonObject["Password"] = password;
        jsonObject["isRegister"] = isRegister;

        socket->write(QJsonDocument(jsonObject).toJson());
    }
}

void MainWindow::on_pushButton_send_clicked()
{
    if (ui->lineEdit_message->text().isEmpty())
        return;

    if (ui->lineEdit_message->text() == "clear_sql"){
        log_sql->clear_table();
        ui->lineEdit_message->clear();
        return;
    }

    if(isServer){
        QString message = QTime::currentTime().toString("hh:mm") + " " + server_name
                + ": " + ui->lineEdit_message->text() + "\n";
        ui->textBrowser->append(message);
        messagesStore->saveMessage(message);

        for ( int i = 0; i < clients.size(); i++) {
            clients[i]->socket->write(message.toUtf8());
        }
    }else {
        socket->write(ui->lineEdit_message->text().toUtf8());
    }

    ui->lineEdit_message->clear();
}

void MainWindow::on_pushButton_login_clicked()
{
    loginedDialog *d = new loginedDialog;
    connect(d, &loginedDialog::data_accept, this, &MainWindow::get_result_from_logined_dialog);
    d->exec();
}

void MainWindow::chat_active()
{
    ui->lineEdit_message->setEnabled(true);

    ui->pushButton_send->setEnabled(true);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            this->hide();
        }
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        minimizeAction->trigger();
        break;
    case QSystemTrayIcon::DoubleClick:
        restoreAction->trigger();
        break;
    default:
        break;
    }
}

void MainWindow::setTrayIconActions()
{
    minimizeAction = new QAction("Свернуть", this);
    restoreAction = new QAction("Восстановить", this);
    quitAction = new QAction("Выход", this);

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);
}

void MainWindow::showTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    QIcon trayImage("title.png");
    trayIcon->setIcon(trayImage);
    trayIcon->show();
}


