#include "jsonhelper.h"

const QString name_key = "name";
const QString password_key = "password";
const QString token_key = "token";

const QString file_name_key = "file_name";
const QString length_key = "length";
const QString state_key = "state";
const QString files_key = "files";
const QString files_dates_key = "files_dates";

bool JSonHelper::is_json(const QByteArray& data)
{
    auto doc = QJsonDocument::fromJson(data);
    if(!doc.isNull()) {
        auto json_obj = doc.object();
        auto json_map = json_obj.toVariantMap();
        token = static_cast<Token>(json_map[token_key].toInt());
        user_name = json_map[name_key].toString();
        user_password = json_map[password_key].toString();
        file_name = json_map[file_name_key].toString();
        file_size = json_map[length_key].toLongLong();
        return true;
    }
    else {
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

JSonHelper::Token JSonHelper::get_token() const
{
    return token;
}

void JSonHelper::clear()
{
    user_name.clear();
    user_password.clear();
    file_name.clear();
    file_size = 0;
}

QByteArray JSonHelper::create_response(JSonHelper::Token token)
{
    QJsonObject obj;
    obj.insert(token_key, static_cast<int>(token));

    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QByteArray JSonHelper::create_list_of_files_response(const QList<std::pair<QString, QString>>& list)
{
    QJsonObject obj;
    obj.insert(token_key, static_cast<int>(Token::list_of_files));
    QJsonArray files_arr;
    QJsonArray files_dates_arr;
    for(int i = 0; i < list.size(); ++i) {
        files_arr.append(list[i].first);
        files_dates_arr.append(list[i].second);
    }
    obj.insert(files_key, files_arr);
    obj.insert(files_dates_key, files_dates_arr);

    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QByteArray JSonHelper::create_download_file_response(qint64 file_size)
{
    QJsonObject obj;
    obj.insert(token_key, static_cast<int>(Token::ready_send_file));
    obj.insert(length_key, file_size);

    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}



















