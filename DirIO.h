#ifndef DIRIO_H
#define DIRIO_H

#include <vector>
#include <QStringList>

namespace io {
    std::vector<QString> GetAllDirectories(QString title, QString root = QString(), QString&& selectedDirectory = QString());

    std::vector<QString> GetAllFiles(QString root, QStringList&& filters = QStringList());

    /**
     * Searches and finds all files recursively from User selected directory
     */
    std::vector<QString> GetAllFiles(QString title, QString root = QString(), QStringList&& filters = QStringList(), QString&& selectedDirectory = QString());

    QString SaveFileTo(QString title, QString dir = QString());

    QString GetName(QString absolutePath);
}
#endif // DIRIO_H
