#ifndef MESSAGESTORE_H
#define MESSAGESTORE_H

#include <QQueue>

class MessageStore
{
public:
    MessageStore();

    void saveMessage(QString message);
    QQueue<QString>* getMessages();

private:
    QQueue<QString> messages;
    int maxMessagesStore = 5;
};

#endif // MESSAGESTORE_H
