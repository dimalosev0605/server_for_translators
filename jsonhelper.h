#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QDebug>

class JSonHelper
{
public:
    enum class Token {
        sing_in = 0,
        sing_up,
        success_sing_in,
        unsuccess_sing_in,
        success_sing_up,
        unsuccess_sing_up,
        internal_server_error,
        upload_file,
        download_file,
        ready_upload_file,
        list_of_files,
        ready_send_file,
        ready_receive_file,
        delete_file
    };

    bool is_json(const QByteArray& data);

    QString get_user_name() const;
    QString get_user_password() const;
    QString get_file_name() const;
    qint64 get_file_size() const;
    Token get_token() const;

    QByteArray create_response(Token token);
    QByteArray create_list_of_files_response(const QList<std::pair<QString, QString>>& list);
    QByteArray create_download_file_response(qint64 file_size);
    void clear();

private:
    QString user_name;
    QString user_password;
    QString file_name;
    qint64 file_size;
    Token token;
};

#endif // JSONHELPER_H
