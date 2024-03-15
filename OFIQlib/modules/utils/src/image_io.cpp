/**
 * @file image_io.cpp
 *
 * @copyright Copyright (c) 2024  Federal Office for Information Security, Germany
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @author OFIQ development team
 */

#include "OFIQError.h"
#include "utils.h"

#include <algorithm>
#include <memory>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace OFIQ;

namespace OFIQ_LIB
{
    OFIQ_EXPORT OFIQ::ReturnStatus readImage(const std::string& filename, OFIQ::Image& image)
    {
        ReturnCode retCode = ReturnCode::Success;
        std::string retStatusInfo{""};

        try
        {
            cv::Mat cvImage = cv::imread(filename, cv::IMREAD_COLOR);
            if(cvImage.empty())
            {
                retCode = ReturnCode::ImageReadingError;
                retStatusInfo = std::string("failed to read image or image is empty: ") + filename;
                return ReturnStatus(retCode, retStatusInfo);
            }

            cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
            image.width = static_cast<uint16_t>(cvImage.cols);
            image.height = static_cast<uint16_t>(cvImage.rows);
            image.depth = 24;

            image.data = std::shared_ptr<uint8_t>(new uint8_t[image.size()]);

            memcpy(image.data.get(), cvImage.data, image.size());
        }
        catch (const std::exception&)
        {
            retCode = ReturnCode::ImageReadingError;
            retStatusInfo = std::string("failed to read image: ") + filename;
            return ReturnStatus(retCode, retStatusInfo);
        }

        return ReturnStatus(retCode, retStatusInfo);
    }
}