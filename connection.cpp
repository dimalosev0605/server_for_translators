#include "connection.h"


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
    qDebug() << user_name;
    qDebug() << user_password;
    qDebug() << static_cast<int>(method);
    qDebug() << file_name;
    qDebug() << "size = " << file_size;
}

void Connection::process_data()
{
    switch (method) {
    case JSonHelper::Method::upload_file: {
        process_upload_file();
        break;
    }
    case JSonHelper::Method::download_file: {
        process_download_file();
        break;
    }
    case JSonHelper::Method::initial: {
        initial();
        break;
    }
    }
}

void Connection::action()
{
    switch (method) {
    case JSonHelper::Method::sign_in: {
        sign_in();
        break;
    }
    case JSonHelper::Method::sign_up: {
        sing_up();
        break;
    }
    case JSonHelper::Method::get_list_of_files: {
        get_list_of_files();
        break;
    }
    case JSonHelper::Method::upload_file: {
        upload_file();
        break;
    }
    case JSonHelper::Method::download_file: {
        download_file();
        break;
    }
    case JSonHelper::Method::delete_file: {
        delete_file();
        break;
    }
    }
}

void Connection::sign_in()
{
    QString qry_str = QString("select user_name, user_password from main where user_name = '%1' and user_password = '%2'").arg(user_name).arg(user_password);
    if(qry.exec(qry_str))
    {
        if(qry.size())
            socket.write(json_helper.create_state_response(JSonHelper::State::success_sing_in));
        else
            socket.write(json_helper.create_state_response(JSonHelper::State::unsuccess_sing_in));
    }
    else
    {
        socket.write(json_helper.create_state_response(JSonHelper::State::internal_server_error));
    }
    method = JSonHelper::Method::initial;
    user_name.clear();
    user_password.clear();
}

void Connection::sing_up()
{
    QString qry_str = QString("insert into main (user_name, user_password) values ('%1', '%2')").arg(user_name).arg(user_password);
    if(qry.exec(qry_str))
    {
        FileManager file_manager;
        if(file_manager.create_dir_for_user(user_name))
            socket.write(json_helper.create_state_response(JSonHelper::State::success_sign_up));
        else
            socket.write(json_helper.create_state_response(JSonHelper::State::internal_server_error));
    }
    else
    {
        socket.write(json_helper.create_state_response(JSonHelper::State::unsuccess_sign_up));
    }
    method = JSonHelper::Method::initial;
    user_name.clear();
    user_password.clear();
}

void Connection::upload_file()
{
    socket.write(json_helper.create_state_response(JSonHelper::State::ready_upload_file));
}

void Connection::process_upload_file()
{
    if(data.size() == file_size) {
        FileManager file_manager;
        QFile file(file_manager.get_file_path(user_name, file_name));
        if(file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();
            socket.write(json_helper.create_state_response(JSonHelper::State::success_uploading));
        }
        else {
            socket.write(json_helper.create_state_response(JSonHelper::State::internal_server_error));
        }
        data.clear();
        method = JSonHelper::Method::initial;
        user_name.clear();
        user_password.clear();
        file_name.clear();
        file_size = 0;
    }
}

void Connection::get_list_of_files()
{
    method = JSonHelper::Method::initial;
    FileManager file_manager;
    socket.write(json_helper.create_get_user_files_response(file_manager.get_list_of_files(user_name)));
}

void Connection::download_file()
{
    FileManager file_manager;
    QFile file(file_manager.get_file_path(user_name, file_name));
    socket.write(json_helper.create_download_file_response(file.size()));
}

void Connection::process_download_file()
{
    if(json_helper.is_json(data))
    {
        state = json_helper.get_state();
        if(state == JSonHelper::State::ready_download_file) {
            FileManager file_manager;
            QFile file(file_manager.get_file_path(user_name, file_name));
            if(file.open(QIODevice::ReadOnly)) {
                socket.write(file.readAll());
                file.close();
            }
            else {
                socket.write(json_helper.create_state_response(JSonHelper::State::internal_server_error));
            }
            method = JSonHelper::Method::initial;
            json_helper.clear();
            data.clear();
            user_name.clear();
            user_password.clear();
            file_name.clear();
            file_size = 0;
        }
    }
}

void Connection::initial()
{
    if(json_helper.is_json(data)) {
        user_name = json_helper.get_user_name();
        user_password = json_helper.get_user_password();
        method = json_helper.get_method();
        file_name = json_helper.get_file_name();
        file_size = json_helper.get_file_size();
        json_helper.clear();
        data.clear();
        action();
    }
}

void Connection::delete_file()
{
    FileManager file_manager;
    QFile file(file_manager.get_file_path(user_name, file_name));
    if(file.remove()) {
        socket.write(json_helper.create_state_response(JSonHelper::State::success_deletion));
    }
    else {
        socket.write(json_helper.create_state_response(JSonHelper::State::internal_server_error));
    }
    data.clear();
    method = JSonHelper::Method::initial;
    user_name.clear();
    user_password.clear();
    file_name.clear();
    file_size = 0;
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
    process_data();
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





