#ifndef SERVER_H
#define SERVER_H

#include <threadconnections.h>
#include <QTcpServer>

class Server: public QTcpServer
{
    Q_OBJECT

    QList<ThreadConnections*> connections;
    int max_threads;
    void divide_load(qintptr socket_descriptor);

public:
    explicit Server(QObject* parent = nullptr);
    void incomingConnection(qintptr socket_descriptor) override;
    void listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
    void set_max_threads(int max_count);
};

#endif // SERVER_H
