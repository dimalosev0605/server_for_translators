#include "filemanager.h"

const QString users_files_dir = "D:/prod_voc/";

bool FileManager::create_dir_for_user(const QString& user_dir_name)
{
    QDir dir(users_files_dir);
    if(dir.mkdir(users_files_dir + user_dir_name))
        return true;
    return false;
}
