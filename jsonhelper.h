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
        initial
    };
    enum class State {
        success_sign_up,
        unsuccess_sign_up,
        internal_server_error,
        success_sing_in,
        unsuccess_sing_in,
        ready_upload_file,
        success_uploading
    };

private:
    QString user_name;
    QString user_password;
    QString file_name;
    qint64 file_size;
    Method method;
public:
    bool is_json(const QByteArray& data);
    QString get_user_name() const;
    QString get_user_password() const;
    QString get_file_name() const;
    qint64 get_file_size() const;
    Method get_method() const;
    void clear();
    QByteArray create_answer(State state);
    QByteArray create_answer_get_user_files(const QList<std::pair<QString, QString>>& list);
};

#endif // JSONHELPER_H
