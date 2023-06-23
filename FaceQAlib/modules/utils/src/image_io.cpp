#include <algorithm>

#include "FaceQaError.h"
#include "utils.h"

#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace FACEQA;

namespace FACEQA_LIB
{
    FACEQA_EXPORT FACEQA::ReturnStatus readImage(const std::string& filename, FACEQA::Image& image)
    {
        ReturnCode retCode = ReturnCode::Success;
        std::string retStatusInfo{""};

        try
        {
            cv::Mat cvImage = cv::imread(filename, cv::IMREAD_COLOR);
            cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
            image.width = cvImage.cols;
            image.height = cvImage.rows;
            image.depth = 24;
#ifdef _WIN32
            image.data = {new uint8_t[image.size()]};
#else
            uint8_t* data = new uint8_t[image.size()];
            image.data.reset(data);
#endif

            memcpy(image.data.get(), cvImage.data, image.size());
        }
        catch (const std::exception&)
        {
            retCode = ReturnCode::ImageReadingError;
            retStatusInfo = std::string("failed to read image: ") + filename;
        }

        return ReturnStatus(retCode, retStatusInfo);
    }
}