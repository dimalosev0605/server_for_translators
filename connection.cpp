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


void Connection::process_data()
{
    if(token == JSonHelper::Token::upload_file) {
        if(data.size() == file_size){
            save_user_file();
            return;
        }
        if(data.size() > file_size) {
            socket.abort();
            return;
        }
        return;
    }
    if(json_helper.is_json(data)) {
        token = json_helper.get_token();
        user_name = json_helper.get_user_name();
        user_password = json_helper.get_user_password();
        file_name = json_helper.get_file_name();
        file_size = json_helper.get_file_size();
        json_helper.clear();
        data.clear();
        response();
    } // else resume receive data...
}

void Connection::response()
{
    switch (token)
    {
    case JSonHelper::Token::sing_in: {
        sing_in_r();
        break;
    }
    case JSonHelper::Token::sing_up: {
        sing_up_r();
        break;
    }
    case JSonHelper::Token::upload_file: {
        upload_file_r();
        break;
    }
    case JSonHelper::Token::list_of_files: {
        send_list_of_files();
        break;
    }
    case JSonHelper::Token::download_file: {
        download_file_r();
        break;
    }
    case JSonHelper::Token::ready_receive_file: {
        send_file();
        break;
    }
    case JSonHelper::Token::delete_file: {
        delete_file_r();
        break;
    }
    }
}

void Connection::sing_in_r()
{
    QString qry_str = QString("select user_name, user_password from main where user_name = '%1' and user_password = '%2'").arg(user_name).arg(user_password);
    if(qry.exec(qry_str)) {
        if(qry.size()) {
            socket.write(json_helper.create_response(JSonHelper::Token::success_sing_in));
        }
        else {
            socket.write(json_helper.create_response(JSonHelper::Token::unsuccess_sing_in));
        }
    }
    else {
        socket.write(json_helper.create_response(JSonHelper::Token::internal_server_error));
    }
    clear();
}

void Connection::sing_up_r()
{
    QString qry_str = QString("insert into main (user_name, user_password) values ('%1', '%2')").arg(user_name).arg(user_password);
    if(qry.exec(qry_str)) {
        FileManager file_manager;
        if(file_manager.create_dir_for_user(user_name)) {
            socket.write(json_helper.create_response(JSonHelper::Token::success_sing_up));
        }
        else {
            socket.write(json_helper.create_response(JSonHelper::Token::internal_server_error));
        }
    }
    else {
        socket.write(json_helper.create_response(JSonHelper::Token::unsuccess_sing_up));
    }
    clear();
}

void Connection::upload_file_r()
{
    socket.write(json_helper.create_response(JSonHelper::Token::ready_upload_file));
}

void Connection::save_user_file()
{
    FileManager file_manager;
    QFile file(file_manager.get_file_path(user_name, file_name));
    if(file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
        FileManager file_manager;
        socket.write(json_helper.create_list_of_files_response(file_manager.get_list_of_files(user_name)));
    }
    else {
        socket.write(json_helper.create_response(JSonHelper::Token::internal_server_error));
    }
    token = static_cast<JSonHelper::Token>(999999); // i do it because in the next process_data() i dont wanna execute token == Token::upload_file case.
    data.clear();
    clear();
}

void Connection::send_list_of_files()
{
    FileManager file_manager;
    socket.write(json_helper.create_list_of_files_response(file_manager.get_list_of_files(user_name)));
}

void Connection::download_file_r()
{
    FileManager file_manager;
    QFile file(file_manager.get_file_path(user_name, file_name));
    socket.write(json_helper.create_download_file_response(file.size()));
}

void Connection::send_file()
{
    FileManager file_manager;
    QFile file(file_manager.get_file_path(user_name, file_name));
    if(file.open(QIODevice::ReadOnly)) {
        socket.write(file.readAll());
        file.close();
    }
    else {
        socket.write(json_helper.create_response(JSonHelper::Token::internal_server_error));
    }
    clear();
}

void Connection::delete_file_r()
{
    FileManager file_manager;
    QFile file(file_manager.get_file_path(user_name, file_name));
    if(file.remove()) {
        send_list_of_files();
    }
    else {
        socket.write(json_helper.create_response(JSonHelper::Token::internal_server_error));
    }
}


void Connection::clear()
{
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





