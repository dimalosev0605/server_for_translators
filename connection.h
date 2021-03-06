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

    QTcpSocket socket;
    QSqlQuery qry;
    JSonHelper json_helper;
    QByteArray data;
    QString user_name;
    QString user_password;
    QString file_name;
    qint64 file_size = 0;
    JSonHelper::Token token;

private:
    void process_data();
    void response();
    void sing_in_r();
    void sing_up_r();
    void upload_file_r();
    void save_user_file();
    void send_list_of_files();
    void download_file_r();
    void send_file();
    void delete_file_r();

    void clear();

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
