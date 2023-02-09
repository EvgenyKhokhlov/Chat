#ifndef SQL_H
#define SQL_H

#include <QtSql>

class sql
{
public:
    sql();
    bool set_user(QString login, QString password);
    bool get_user(QString login, QString password);
    void clear_table();
};

#endif // SQL_H
