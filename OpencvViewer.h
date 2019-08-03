#pragma once

#include <QObject>

#include "opencv2/imgproc.hpp"

constexpr auto WINDOW_1 = "img1";
constexpr auto WINDOW_2 = "img2";

typedef std::string ImageWindow;

struct ClickSlice
{
    std::string path;
    ClickSlice() {};
    ClickSlice(int x, int y, std::string iw) : x(x), y(y), imageWindow(iw) {}
    int x = 0;
    int y = 0;
    int size = 0;
    ImageWindow imageWindow;
};

void mouseCallBackFunction1(int event, int x, int y, int flags, void* data);
void mouseCallBackFunction2(int event, int x, int y, int flags, void* data);

class CV_Viewer : public QObject
{
    Q_OBJECT
public:
    CV_Viewer();
    ~CV_Viewer();

    void mouseCallBackFunction(int event, int x, int y, int flags, void* data);

public slots:
    void showImageFromFileToView1(std::string path, cv::Mat image);
    void showImageFromFileToView2(std::string path, cv::Mat image);
    void setSize(int size);

signals:
    void sliceSelected(cv::Mat img, cv::Mat histogram, ClickSlice slice);

private:
    std::string winname1 = "";
    std::string winname2 = "";

    std::string path1_ = "";
    std::string path2_ = "";

    cv::Mat image1;
    cv::Mat image2;
    int size_ = 0;

};