#include "threadconnections.h"

ThreadConnections::ThreadConnections(QObject *parent)
    : QObject (parent)
{
    qDebug() << this << " created, thread: " << QThread::currentThreadId();
    QThread* thread = new QThread(this);
    connect(this, &ThreadConnections::new_connection, this, &ThreadConnections::accept_socket_descriptor,
            Qt::QueuedConnection);
    this->moveToThread(thread);
    thread->start(QThread::Priority::TimeCriticalPriority);
}

void ThreadConnections::open_db(QSqlDatabase &db, const QString &name, const QString &host_name, const QString &user_name, const QString &password, int port)
{
    QString connection_name = QString::number((quint64)QThread::currentThread(), 16);
    qDebug() << "connection_name = " << connection_name;
    db = QSqlDatabase::addDatabase("QPSQL", connection_name);

    db.setDatabaseName(name);
    db.setHostName(host_name);
    db.setUserName(user_name);
    db.setPassword(password);
    db.setPort(port);
    if(db.open())
    {
        qDebug() << " Databese " << name << "opened in thread " << QThread::currentThreadId();
    }
    else
    {
        qDebug() << " Database " << name << "not opened in thread " << QThread::currentThreadId();
        qDebug() << db.lastError().text();
    }
}

void ThreadConnections::decrease_count_of_connections()
{
    --count_of_connections;
    qDebug() << this << " decrease count of connections to: " << count_of_connections << ", thread: " << QThread::currentThreadId();
}


void ThreadConnections::accept_socket_descriptor(qintptr socket_descriptor)
{
    if(!db.isOpen())
    {
        open_db(db, "prod_voc", "localhost", "postgres", "4952211285", 5432);
    }

    ++count_of_connections;
    qDebug() << this << " increase count of connections to: " << count_of_connections << ", thread: " << QThread::currentThreadId();

    Connection* new_connection = new Connection(socket_descriptor, db);
    connect(new_connection, &QObject::destroyed, this, &ThreadConnections::decrease_count_of_connections);
}

int ThreadConnections::get_count_of_connections() const
{
    return count_of_connections;
}
