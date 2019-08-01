#pragma once


#include <map>
#include <QString>
#include <QStringList>
#include <opencv2/core/mat.hpp>

namespace histology {

    class CancerType 
    {
    public:
        CancerType();
        CancerType(QString name, QString&& absPath);
        ~CancerType();
    
        QString getCancerTypeName() const;
        friend class Dataset;
    private:
        QString name_;
        QString absPath_;
        QStringList imageFilePaths_;
    
    };


    class Dataset
    {
    public:

        Dataset();
        ~Dataset();

        bool loadDataset(QString path = QString());
        QStringList getCancerTypes();
        QStringList getCancerTypeImagesList(QString type);

        cv::Mat readImage(const QString path);

    private:
        QString location_;
        std::map<QString, CancerType> classes_;

        cv::Mat lastLoadedImage;

    };

}
