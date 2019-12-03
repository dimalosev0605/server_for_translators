#include "filemanager.h"

const QString users_files_dir = "D:/prod_voc/";
const QString abk_extension = ".abk";

bool FileManager::create_dir_for_user(const QString& user_dir_name)
{
    QDir dir(users_files_dir);
    if(dir.mkdir(users_files_dir + user_dir_name))
        return true;
    return false;
}

QString FileManager::get_file_path(const QString& user_name, const QString& file_name)
{
    return users_files_dir + user_name + '/' + file_name + abk_extension;
}

QList<std::pair<QString, QString>> FileManager::get_list_of_files(const QString &user_name)
{
    QDir dir(users_files_dir + user_name);
    dir.setNameFilters(QStringList {'*' + abk_extension} );
    dir.setSorting(QDir::SortFlag::Time);
    auto list = dir.entryInfoList();
    QList<std::pair<QString, QString>> files;
    for(int i = 0; i < list.size(); ++i)
        files.push_back({{list[i].baseName()},{list[i].lastModified().toString()}});
    return files;
}
