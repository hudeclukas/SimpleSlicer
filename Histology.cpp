#include "Histology.h"

#include "DirIO.h"

#include  <QStringList>
#include <opencv2/imgcodecs.hpp>

histology::CancerType::CancerType()
= default;

histology::CancerType::CancerType(QString name, QString&& absPath) : name_(name), absPath_(absPath)
{
}

histology::CancerType::~CancerType()
{
}


QString histology::CancerType::getCancerTypeName() const
{
    return name_;
}


histology::Dataset::Dataset()
{
    loadDataset(QString());
}

histology::Dataset::~Dataset()
= default;

bool histology::Dataset::loadDataset(QString path)
{
    std::vector<QString> classSubDirs = io::GetAllDirectories("Select parent folder", path, std::move(location_));
    if (classSubDirs.empty())
    {
        return false;
    }
    classes_.clear();
    for (auto it = classSubDirs.begin()+2; it < classSubDirs.end(); ++it)
    {
        QString fName = io::GetName(*it);
        CancerType ct = CancerType(fName, std::move(*it));
        auto files = io::GetAllFiles(*it, std::move(QStringList() << "*.tiff" << "*.tif" << "*.png" << "*.jpg"));
        for (auto f : files)
        {
            ct.imageFilePaths_ << f;
        }
        classes_[fName] = ct;
    }
    return !classes_.empty();
}

QStringList histology::Dataset::getCancerTypes()
{
    QStringList cancerTypes;
    for (auto type : classes_)
    {
        cancerTypes << type.second.getCancerTypeName();
    }
    return cancerTypes;
}

QStringList histology::Dataset::getCancerTypeImagesList(QString type)
{
    return classes_[type].imageFilePaths_;
}

cv::Mat histology::Dataset::readImage(const QString path)
{
    lastLoadedImage = cv::imread(path.toStdString(), cv::IMREAD_ANYCOLOR);
    return lastLoadedImage;
}
