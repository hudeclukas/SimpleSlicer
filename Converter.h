#ifndef CONVERTER_H
#define CONVERTER_H

#include <QtGui/qimage.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/shape/hist_cost.hpp>


inline QImage Mat2QImage(cv::Mat src)
{ 
    switch (src.type())
    {
        // 8-bit, 4 channel
        case CV_8UC4:
        {
            QImage dest(src.data, src.cols, src.rows, src.step, QImage::Format_RGBA8888);

            return dest.copy();
        }

        // 8-bit, 3 channel
        case CV_8UC3:
        {
            QImage dest(src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);

            return dest.copy();
        }

        // 16-bit, 1 channel
        case CV_16U:
        {
            QImage dest(src.data, src.cols, src.rows, src.step, QImage::Format_RGB16);

            return dest.copy();
        }

        // 16-bit, 3 channel
        case CV_16UC3:
        {
            cv::Mat img(src.rows, src.cols, CV_16UC4, { 65535, 65535 ,65535 ,65535 });
            for (auto row = 0; row < src.rows; ++row)
            {
                auto srcptr = src.ptr<cv::Vec3w>(row);
                auto imgptr = img.ptr<cv::Vec4w>(row);
                for (auto col = 0; col < src.cols; ++col)
                {
                    imgptr[col][0] = srcptr[col][0];
                    imgptr[col][1] = srcptr[col][1];
                    imgptr[col][2] = srcptr[col][2];
                }
            }
            QImage dest(img.data, img.cols, img.rows, img.step, QImage::Format_RGBX64);

            return dest.copy();
        }

        // 8-bit, 1 channel
        case CV_8UC1:
        {
            QImage dest(src.data, src.cols, src.rows, src.step, QImage::Format_Grayscale8);
            return dest.copy();
        }

        default:

            break;
    }
    return QImage(src.cols, src.rows, QImage::Format_Mono);
}
#endif // CONVERTER_H
