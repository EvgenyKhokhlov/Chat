#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Dialog\dialog.h"
#include "Dialog\logineddialog.h"
#include "messagestore.h"
#include "sql.h"

#include <QMainWindow>
#include <QObject>
#include <QTcpServer>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTime>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>

#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct Client{
        QTcpSocket* socket;
        QString name;
        bool logined = false;
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    int port;
    QString host;
    QString server_name;
    bool isServer = false;
    bool isClientLogined = false;

    QVector<Client*> clients;
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *socket;
    bool init_server();
    bool init_client();
    void get_message();
    QString send_message_on_client_logined(Client* client, bool isRegister);
    void client_ready_read();
    void chat_active();
    void open_dialog();

    sql *log_sql;
    MessageStore* messagesStore;

    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;

private slots:
    void client_connected();
    void client_connected_to_server();
    void on_pushButton_init_server_clicked();
    void on_pushButton_send_clicked();
    void on_pushButton_connect_client_clicked();
    void get_result_from_dialog(QString dialogHost, int dialogPort);
    void get_result_from_logined_dialog(QString login, QString password, bool isRegister);
    void on_pushButton_login_clicked();

    void changeEvent(QEvent* event);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setTrayIconActions();
    void showTrayIcon();
};
#endif // MAINWINDOW_H
