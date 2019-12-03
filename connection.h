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
    JSonHelper::Method method = JSonHelper::Method::initial;
    QString file_name;
    qint64 file_size;
    JSonHelper::State state;

private:
    void log();
    void process_data(const QByteArray& data);
    void action();
    void sign_in();
    void sing_up();
    void upload_file();
    void process_upload_file();
    void get_list_of_files();
    void download_file();
    void process_download_file(const QByteArray& data);

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
