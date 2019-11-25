#ifndef THREADCONNECTIONS_H
#define THREADCONNECTIONS_H

#include <connection.h>
#include <QSqlDatabase>
#include <QSqlError>

class ThreadConnections: public QObject
{
    Q_OBJECT

    QSqlDatabase db;
    int count_of_connections = 0;

private:
    void open_db(QSqlDatabase& db, const QString& name, const QString& host_name,
                 const QString& user_name, const QString& password, int port);

private slots:
    void decrease_count_of_connections();

public:
    explicit ThreadConnections(QObject* parent = nullptr);

public slots:
    void accept_socket_descriptor(qintptr socket_descriptor);
    int get_count_of_connections() const;

signals:
    void new_connection(qintptr socket_descriptor);
};

#endif // THREADCONNECTIONS_H
