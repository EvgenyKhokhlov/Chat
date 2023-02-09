#include "sql.h"

sql::sql()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("data.db");
    bool ok = db.open();

    if(!ok) qInfo() << db.lastError().databaseText();

    QSqlQuery a_query;
    QString str = "CREATE TABLE IF NOT EXISTS login_info ("
            "login VARCHAR(255), "
            "password VARCHAR(255)"
            ");";
    bool b = a_query.exec(str);
    if (!b) qInfo() << "Table not created";
}

bool sql::set_user(QString login, QString password)
{
    QSqlQuery a_query;
    QSqlRecord rec = a_query.record();

    while (a_query.next()) {
        if (login == a_query.value(rec.indexOf("login")).toString()) {
            return false;
            break;
        }
    }

    QString str_insert = "INSERT INTO login_info(login, password) "
            "VALUES ('%1', '%2');";
    QString str = str_insert.arg(login)
            .arg(password);
    bool b = a_query.exec(str);
    if (!b) qInfo() << "Data not set";
    return true;
}

bool sql::get_user(QString login, QString password)
{
    QSqlQuery a_query;
    if (!a_query.exec("SELECT * FROM login_info")) qInfo() << "Data not get";

    QSqlRecord rec = a_query.record();

    while (a_query.next()) {
        if (login == a_query.value(rec.indexOf("login")).toString()
                && password == a_query.value(rec.indexOf("password")).toString()) {
            return true;
            break;
        }
    }
    return false;
}

void sql::clear_table()
{
    QSqlQuery a_query;
    QString str = "DROP TABLE IF NOT EXISTS login_info";
    bool b = a_query.exec(str);
    if (!b) qInfo() << "Table not cleared";
}
