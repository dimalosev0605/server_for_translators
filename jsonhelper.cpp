#include "jsonhelper.h"

const QString name_key = "name";
const QString password_key = "password";
const QString method_key = "method";
const QString file_name_key = "file_name";
const QString length_key = "length";
const QString state_key = "state";
const QString files_key = "files";
const QString files_dates_key = "files_dates";

bool JSonHelper::is_json(const QByteArray& data)
{
    auto doc = QJsonDocument::fromJson(data);
    if(doc.isObject())
    {
        auto json_obj = doc.object();
        auto json_map = json_obj.toVariantMap();
        user_name = json_map[name_key].toString();
        user_password = json_map[password_key].toString();
        method = static_cast<Method>(json_map[method_key].toInt());
        file_name = json_map[file_name_key].toString();
        file_size = json_map[length_key].toLongLong();
        state = static_cast<State>(json_map[state_key].toInt());
        return true;
    }
    else
    {
        return false;
    }
}

QString JSonHelper::get_user_name() const
{
    return user_name;
}

QString JSonHelper::get_user_password() const
{
    return user_password;
}

QString JSonHelper::get_file_name() const
{
    return file_name;
}

qint64 JSonHelper::get_file_size() const
{
    return file_size;
}

JSonHelper::Method JSonHelper::get_method() const
{
    return method;
}

JSonHelper::State JSonHelper::get_state() const
{
    return state;
}

void JSonHelper::clear()
{
    user_name.clear();
    user_password.clear();
    file_name.clear();
    file_size = 0;
}

QByteArray JSonHelper::create_answer(JSonHelper::State state)
{
    QJsonObject obj;
    obj.insert(state_key, QJsonValue::fromVariant(static_cast<int>(state)));
    QJsonDocument doc(obj);
    return doc.toJson();
}

QByteArray JSonHelper::create_answer_get_user_files(const QList<std::pair<QString, QString>>& list)
{
    QJsonObject obj;
    QJsonArray files;
    QJsonArray files_dates;
    for(int i = 0; i < list.size(); ++i) {
        files.append(list[i].first);
        files_dates.append(list[i].second);
    }
    obj.insert(files_key, files);
    obj.insert(files_dates_key, files_dates);
    QJsonDocument doc(obj);
    return doc.toJson();
}

QByteArray JSonHelper::create_answer_download_file(qint64 size)
{
    QJsonObject obj;
    obj.insert(length_key, QJsonValue::fromVariant(size));
    QJsonDocument doc(obj);
    return doc.toJson();
}











