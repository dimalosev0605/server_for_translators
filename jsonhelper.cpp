#include "jsonhelper.h"

const QString state_key = "state";

bool JSonHelper::is_json_obj(const QByteArray &data)
{
    auto json_doc = QJsonDocument::fromJson(data);
    if(json_doc.isObject())
    {
        auto json_obj = json_doc.object();
        auto json_map = json_obj.toVariantMap();
        method = static_cast<Method>(json_map["method"].toInt());
        switch (method) {
        case Method::sign_in:
        {
        }
        case Method::sign_up:
        {
            user_name = json_map["name"].toString();
            user_password = json_map["password"].toString();
            return true;
        }
        }
//        file = json_map["file_name"].toString();
//        length = json_map["length"].toInt();
    }
    else
    {
        qDebug() << "not object. Resume receiving";
        return false;
    }
}

QByteArray JSonHelper::create_sing_up_answer(const QString &state)
{
    QJsonObject obj;
    obj.insert(state_key, QJsonValue::fromVariant(state));
    QJsonDocument doc(obj);
    return doc.toJson();
}

QString JSonHelper::get_user_name() const
{
    return user_name;
}

QString JSonHelper::get_user_password() const
{
    return user_password;
}

JSonHelper::Method JSonHelper::get_method() const
{
    return method;
}

QString JSonHelper::get_file() const
{
    return file;
}

qint64 JSonHelper::get_length() const
{
    return length;
}







