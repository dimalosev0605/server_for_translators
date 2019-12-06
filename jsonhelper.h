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
    enum class Method {
        sign_up,
        sign_in,
        get_list_of_files,
        download_file,
        upload_file,
        initial,
        delete_file
    };
    enum class State {
        success_sign_up,
        unsuccess_sign_up,
        internal_server_error,
        success_sing_in,
        unsuccess_sing_in,
        ready_upload_file,
        success_uploading,
        ready_download_file,
        success_deletion
    };

private:
    QString user_name;
    QString user_password;
    QString file_name;
    qint64 file_size;
    Method method;
    State state;
public:
    bool is_json(const QByteArray& data);
    QString get_user_name() const;
    QString get_user_password() const;
    QString get_file_name() const;
    qint64 get_file_size() const;
    Method get_method() const;
    State get_state() const;
    void clear();
    QByteArray create_state_response(State state);
    QByteArray create_get_user_files_response(const QList<std::pair<QString, QString>>& list);
    QByteArray create_download_file_response(qint64 size);
};

#endif // JSONHELPER_H
