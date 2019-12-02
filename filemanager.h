#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QDir>
#include <QCoreApplication>
#include <QDateTime>

class FileManager
{
public:
    bool create_dir_for_user(const QString& user_dir_name);
    QString get_file_path(const QString& user_name, const QString& file_name);
    QList<std::pair<QString, QString>> get_list_of_files(const QString& user_name);
};

#endif // FILEMANAGER_H
