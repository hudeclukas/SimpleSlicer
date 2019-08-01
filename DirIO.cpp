#include "DirIO.h"

#include <QFileDialog>
#include <QDirIterator>

std::vector<QString> io::GetAllDirectories(QString title, QString root, QString&& selectedDirectory)
{
    selectedDirectory = QFileDialog::getExistingDirectory(nullptr, title, root, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (selectedDirectory.isEmpty())
    {
       return std::vector<QString>();
    }
    QDirIterator dit(selectedDirectory);
    std::vector<QString> dirs;
    while (dit.hasNext())
    {
        dirs.push_back(dit.next());
    }
    return dirs;
}

std::vector<QString> io::GetAllFiles(QString root, QStringList&& filters)
{
    QDirIterator it(root, filters, QDir::Files);
    std::vector<QString> files;
    while (it.hasNext())
    {
        files.push_back(it.next());
    }
    return files;
}

std::vector<QString> io::GetAllFiles(QString title, QString root, QStringList &&filters, QString &&selectedDirectory)
{
    selectedDirectory = QFileDialog::getExistingDirectory(nullptr, title, root, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (selectedDirectory.isEmpty())
    {
        return std::vector<QString>();
    }

    QDirIterator it(selectedDirectory, filters, QDir::Files, QDirIterator::Subdirectories);
    std::vector<QString> files;
    while (it.hasNext())
    {
        files.push_back(it.next());
    }
    return files;
}

QString io::SaveFileTo(QString title, QString dir)
{
    return QFileDialog::getSaveFileName(nullptr, title, dir);
}

QString io::GetName(QString absolutePath)
{
    QFileInfo fi(absolutePath);
    return fi.baseName();
}