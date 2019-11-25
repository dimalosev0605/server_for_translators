#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThread>
#include <QSqlQuery>
#include "jsonhelper.h"
#include "filemanager.h"

class Connection : public QObject
{
    Q_OBJECT

    enum class Method {
        sign_up,
        sign_in
    };

    QTcpSocket socket;
    QSqlQuery qry;
    JSonHelper json_helper;
    QByteArray data;
    QString user_name;
    QString user_password;
    Method method;
    QString file;
    qint64 length;

private:
    void log(); // temporary function
    void cater_client();
    void sign_up();
    void sign_in();

private slots:
    void connected();
    void disconnected();
    void ready_read();
    void bytes_written(qint64 bytes);
    void stateChanged(QAbstractSocket::SocketState socketState);
    void hostFound();
    void error(QAbstractSocket::SocketError socketError);

public:
    explicit Connection(qint64 socket_descriptor, const QSqlDatabase& db, QObject *parent = nullptr);
    ~Connection();

public slots:
};

#endif // CONNECTION_H
