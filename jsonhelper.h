#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QDebug>

class JSonHelper
{
    enum class Method {
        sign_up,
        sign_in
    };

    QString user_name;
    QString user_password;
    Method method;
    QString file;
    qint64 length;

public:
    bool is_json_obj(const QByteArray& data);
    QByteArray create_sing_up_answer(const QString& state);
    QString get_user_name() const;
    QString get_user_password() const;
    Method get_method() const;
    QString get_file() const;
    qint64 get_length() const;
};

#endif // JSONHELPER_H
