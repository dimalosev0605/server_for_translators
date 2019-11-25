#include "server.h"

Server::Server(QObject* parent)
    : QTcpServer (parent)
{
    qDebug() << this << " created, thread: " << QThread::currentThreadId();
    max_threads = QThread::idealThreadCount();
    qDebug() << "count of threads: " << max_threads;
}


void Server::set_max_threads(int max_count)
{
    max_threads = max_count;
    qDebug() << this << " Count of threads is set to: " << max_threads;
}


void Server::listen(const QHostAddress& address, quint16 port)
{
    if(QTcpServer::listen(address, port))
    {
        qDebug() << this << " started on port:"
                 << QTcpServer::serverPort();

        for(int i = 0; i < max_threads; ++i) {
            ThreadConnections* new_connections = new ThreadConnections;
            connections.append(new_connections);
        }
    }
    else
    {
        qDebug() << this << " did no start";
    }
}

void Server::incomingConnection(qintptr socket_descriptor)
{
    divide_load(socket_descriptor);
}

void Server::divide_load(qintptr socket_descriptor)
{
    int min_c = connections[0]->get_count_of_connections();
    int n = 0;

    for(int i = 0; i < max_threads; ++i) {
        if(connections[i]->get_count_of_connections() < min_c) {
            min_c = connections[i]->get_count_of_connections();
            n = i;
        }
    }

    qDebug() << this << " min load thread number is: " << n;
    emit connections[n]->new_connection(socket_descriptor);
}













