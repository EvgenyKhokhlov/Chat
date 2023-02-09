#include "messagestore.h"

MessageStore::MessageStore()
{

}

void MessageStore::saveMessage(QString message)
{
    if (messages.size() >= maxMessagesStore)
        messages.dequeue();

    messages.enqueue(message);
}

QQueue<QString> *MessageStore::getMessages()
{
    return &messages;
}
