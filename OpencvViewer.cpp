#include "OpencvViewer.h"

#include "opencv2/highgui.hpp"

CV_Viewer::CV_Viewer()
{
}

CV_Viewer::~CV_Viewer()
{
}

void CV_Viewer::mouseCallBackFunction(int event, int x, int y, int flags, void* data)
{
    auto* cs = static_cast<ClickSlice*>(data);
    std::string pt;
    if (flags==CV_EVENT_FLAG_LBUTTON)
    {
        cv::Mat* image = nullptr;
        if (cs->imageWindow==WINDOW_1)
        {
            image = new cv::Mat();
            image1.copyTo(*image);
            pt = path1_;
        }
        else if (cs->imageWindow==WINDOW_2)
        {
            image = new cv::Mat();
            image2.copyTo(*image);
            pt = path2_;
        }

        const int shift = size_ / 2;
       
        cv::Mat slice = cv::Mat::zeros(size_, size_, image->type());
        int hist_bins = 256;
        std::vector<cv::Vec3i> hst(hist_bins);
        cv::Vec3i maxcounts = { 0,0,0 };

        if (image->type() == CV_16UC3) {
            for (auto row = 0; row < size_; ++row)
            {
                auto imgrow = image->ptr<cv::Vec3w>(y <= shift ? row : y + shift <= image->rows ? y - shift + row : image->rows - size_ + row);
                auto slicerow = slice.ptr<cv::Vec3w>(row);
                for (auto col = 0; col < size_; ++col)
                {
                    slicerow[col] = imgrow[x <= shift ? col : x + shift <= image->cols ? x - shift + col : image->cols - size_ + col];
                    float pixel = (float(slicerow[col][0]) / 65535.f) * float(hist_bins - 1);
                    if (++hst[pixel][0] > maxcounts[0]) maxcounts[0]++;
                    pixel = (float(slicerow[col][1]) / 65535.f) * float(hist_bins - 1);
                    if (++hst[pixel][1] > maxcounts[1]) maxcounts[1]++;
                    pixel = (float(slicerow[col][2]) / 65535.f) * float(hist_bins - 1);
                    if (++hst[pixel][2] > maxcounts[2]) maxcounts[2]++;
                }
            }

            cv::Mat histogram = cv::Mat::zeros(200, hist_bins, CV_8UC3);
            cv::Point pointB_prev = cv::Point(0, 199);
            cv::Point pointG_prev = cv::Point(0, 199);
            cv::Point pointR_prev = cv::Point(0, 199);
            for (auto col = 0; col < hist_bins; ++col)
            {
                cv::Point pointB = cv::Point(col, 199 - ((float(hst[col][0]) / float(maxcounts[0])) * 199));
                cv::line(histogram, pointB_prev, pointB, { 0,0,255 });
                cv::Point pointG = cv::Point(col, 199 - ((float(hst[col][1]) / float(maxcounts[1])) * 199));
                cv::line(histogram, pointG_prev, pointG, { 0,255,0 });
                cv::Point pointR = cv::Point(col, 199 - ((float(hst[col][2]) / float(maxcounts[2])) * 199));
                cv::line(histogram, pointR_prev, pointR, { 255,0,0 });
                pointB_prev = pointB;
                pointG_prev = pointG;
                pointR_prev = pointR;
            }

            cs->size = size_;
            cs->path = pt;
            emit this->sliceSelected(slice, histogram, *cs);
        }
        cv::rectangle(*image, cv::Point(x < shift ? 0 : x - shift, y < shift? 0 : y - shift), cv::Point(x + shift > image->cols ? image->cols : x + shift, y + shift > image->rows ? image->rows : y + shift), cv::Scalar(0, 0xffff, 0), 4, cv::LINE_8, 0);
        cv::imshow(cs->imageWindow, *image);

        delete image;
    }
}

void CV_Viewer::showImageFromFileToView1(std::string path, cv::Mat image)
{
    if (winname1.empty())
    {
        winname1 = WINDOW_1;
        cv::namedWindow(winname1, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    }
    cv::setWindowTitle(winname1, "Class 1 - " + path);
    image1 = image;
    path1_ = path;
    cv::setMouseCallback(winname1, mouseCallBackFunction1, this);
    cv::imshow(winname1, image);
}

void CV_Viewer::showImageFromFileToView2(std::string path, cv::Mat image)
{
    if (winname2.empty())
    {
        winname2 = WINDOW_2;
        cv::namedWindow(winname2, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    } 
    cv::setWindowTitle(winname2, "Class 2 - " + path);
    image2 = image;
    path2_ = path;
    cv::setMouseCallback(winname2, mouseCallBackFunction2, this);
    cv::imshow(winname2, image);
}

void CV_Viewer::setSize(int size)
{
    size_ = size;
}

void mouseCallBackFunction1(int event, int x, int y, int flags, void* data)
{
    auto cvv = static_cast<CV_Viewer*>(data);
    ClickSlice cs(x, y, WINDOW_1);
    cvv->mouseCallBackFunction(event, x, y, flags, &cs);
}

void mouseCallBackFunction2(int event, int x, int y, int flags, void* data)
{
    auto cvv = static_cast<CV_Viewer*>(data);
    ClickSlice cs(x, y, WINDOW_2);
    cvv->mouseCallBackFunction(event, x, y, flags, &cs);
}