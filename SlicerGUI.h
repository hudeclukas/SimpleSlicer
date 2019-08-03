#pragma once

#include <QMainWindow>
#include <opencv2/core/mat.hpp>
#include "OpencvViewer.h"

namespace histology
{
    class Dataset;
}

class QListWidgetItem;

class Ui_SimpleGUI;

struct Slice : ClickSlice
{
    Slice() {}
    Slice(ClickSlice &cls, cv::Mat &slice_, cv::Mat &histogram_): ClickSlice(cls.x, cls.y, cls.imageWindow)
    {
        path = cls.path;
        slice_.copyTo(slice);
        histogram_.copyTo(histogram);
    }

    QString type;
    cv::Mat slice;
    cv::Mat histogram;
};

struct SlicePairNames
{
    SlicePairNames() {}
    SlicePairNames(QString &&name1, QString &&name2) : name1(name1), name2(name2) {}
    QString getCSVLine()
    {
        return name1 + ","+ name2;
    }
    QString getCSVLineTif()
    {
        return name1 + ".tif," + name2 + ".tif";
    }
    QString name1;
    QString name2;
};

class SlicerGUI : public QMainWindow
{
    Q_OBJECT
public:
    SlicerGUI();
    ~SlicerGUI();

public slots:
    void on_actionLoad_Directory_triggered();
    void on_actionExit_triggered();

    void onChange_ComboBox1(const QString& text);
    void onChange_ComboBox2(const QString& text);

    void onChange_ClassList1();
    void onChange_ClassList2();

    void onChange_PairsList();

    void onEditSliceName();
    void on_keepBtn_clicked();
    void on_removeBtn_clicked();
    void on_saveAll_clicked();

    void updateNameEdit(QString file1_info = QString(), QString file2_info = QString());
    void recieveSlice(cv::Mat img, cv::Mat histogram, ClickSlice slice);

private:
    Ui_SimpleGUI* ui;
    CV_Viewer* viewer_;
    histology::Dataset* dataset_histology_;

    std::map<QString, Slice> slices_;
    std::map<QString, SlicePairNames> slicePairNames_;

    int lastID1 = 0;
    int lastID2 = 0;

    Slice* viewedSlice1_ = nullptr;
    Slice* viewedSlice2_ = nullptr;
    
    bool flagSlice1Changed_ = false;
    bool flagSlice2Changed_ = false;

    bool flagKept_ = false;

    bool nameEdited_ = false;
};