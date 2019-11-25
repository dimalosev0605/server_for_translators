#include "connection.h"

const QString success_sign_up = "success sign up";
const QString sign_up_conflict = "sign up conflict";

Connection::Connection(qint64 socket_descriptor, const QSqlDatabase& db, QObject *parent)
    : QObject(parent)
{
    qDebug() << this << " created, thread: " << QThread::currentThreadId();

    connect(&socket, &QTcpSocket::hostFound, this, &Connection::hostFound);
    connect(&socket, &QTcpSocket::connected, this, &Connection::connected);
    connect(&socket, &QTcpSocket::stateChanged, this, &Connection::stateChanged);
    connect(&socket, &QTcpSocket::readyRead, this, &Connection::ready_read);
    connect(&socket, &QTcpSocket::bytesWritten, this, &Connection::bytes_written);
    connect(&socket, &QTcpSocket::disconnected, this, &Connection::disconnected);
    connect(&socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Connection::error);

    socket.setSocketDescriptor(socket_descriptor);
    qry = QSqlQuery(db);
}

Connection::~Connection()
{
    qDebug() << this << " destroyed, thread: " << QThread::currentThreadId();
}

void Connection::log()
{
    qDebug() << "user_name = " << user_name;
    qDebug() << "user_password = " << user_password;
    qDebug() << "method = " << static_cast<int>(method);
    qDebug() << "file = " << file;
    qDebug() << "length = " << length;
}

void Connection::cater_client()
{
    switch (method)
    {
    case Method::sign_in:
    {
        sign_in();
        break;
    }
    case Method::sign_up:
    {
        sign_up();
        break;
    }
    }
}

void Connection::sign_up()
{
    qDebug() << "IN sign up";
    log();
    QString str_qry(QString("insert into main (user_name, user_password) values ('%1', '%2')").arg(user_name).arg(user_password));
    if(qry.exec(str_qry))
    {
        // success sign up
        // need close socket+, create user folder-, write successful notification+.
        FileManager file_manager;
        if(file_manager.create_dir_for_user(user_name))
        {
            socket.write(json_helper.create_sing_up_answer(success_sign_up));
            socket.close();
        }
        else
        {
            // write internal server error.
        }
    }
    else
    {
        // such user name already exists.
        socket.write(json_helper.create_sing_up_answer(sign_up_conflict));
        user_name.clear();
        user_password.clear();
    }
}

void Connection::sign_in()
{
    qDebug() << "IN sign in";
    log();
}

void Connection::connected()
{
    qDebug() << this << " connected, thread: " << QThread::currentThreadId();
}

void Connection::disconnected()
{
    qDebug() << this << " disconnected, thread: " << QThread::currentThreadId();
    deleteLater();
}

void Connection::ready_read()
{
    qDebug() << this << " ready_read, thread: " << QThread::currentThreadId();
    qint64 bytes_available = socket.bytesAvailable();
    data += socket.read(bytes_available);
    if(json_helper.is_json_obj(data))
    {
        method = static_cast<Method>(json_helper.get_method());
        user_name = json_helper.get_user_name();
        user_password = json_helper.get_user_password();
        data.clear();
        cater_client();
    }
    else
    {
        if(data.size() > 1024 * 1024) socket.abort();
    }
}

void Connection::bytes_written(qint64 bytes)
{
    qDebug() << this << " written: " << bytes << " bytes, thread: " << QThread::currentThreadId();
}

void Connection::stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << this << " state changed to " << socketState << " thread: " << QThread::currentThreadId();
    switch (socketState) {
    case QAbstractSocket::SocketState::HostLookupState:
    {
        break;
    }
    case QAbstractSocket::SocketState::ConnectedState:
    {
        break;
    }
    case QAbstractSocket::SocketState::ConnectingState:
    {
        break;
    }
    case QAbstractSocket::SocketState::UnconnectedState:
    {
        break;
    }
    case QAbstractSocket::SocketState::BoundState:
    {
        break;
    }
    case QAbstractSocket::SocketState::ClosingState:
    {
        break;
    }
    }
}

void Connection::hostFound()
{
    qDebug() << this << " host found, thread: " << QThread::currentThreadId();
}

void Connection::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << this << " error " << socketError << " thread: " << QThread::currentThreadId();
    switch (socketError) {
    case QAbstractSocket::SocketError::ConnectionRefusedError:
    {
        break;
    }
    }
}





