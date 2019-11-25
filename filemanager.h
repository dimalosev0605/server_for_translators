#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QDir>
#include <QCoreApplication>

class FileManager
{
public:
    bool create_dir_for_user(const QString& user_dir_name);
};

#endif // FILEMANAGER_H
