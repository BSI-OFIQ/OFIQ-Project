/**
 * @file RuntimeThreadSettings.cpp
 *
 * @copyright Copyright (c) 2026  Federal Office for Information Security, Germany
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

#include "RuntimeThreadSettings.h"

#include "OFIQError.h"

#include <cmath>
#include <limits>
#include <opencv2/core/utility.hpp>

namespace OFIQ_LIB
{
    namespace
    {
        int GetDefaultCvThreadCount()
        {
            static const int defaultCvThreadCount = cv::getNumThreads();
            return defaultCvThreadCount;
        }
    }

    unsigned int RuntimeThreadSettings::GetConfiguredThreadCount(
        const Configuration& config,
        const char* key)
    {
        double configuredValue = 0.0;
        if (!config.GetNumber(key, configuredValue))
            return 0;

        if (!std::isfinite(configuredValue) || configuredValue < 0.0 ||
            std::trunc(configuredValue) != configuredValue ||
            configuredValue > static_cast<double>(std::numeric_limits<unsigned int>::max()))
        {
            throw OFIQError(
                OFIQ::ReturnCode::UnknownConfigParamError,
                std::string(key) + " must be a non-negative integer");
        }

        return static_cast<unsigned int>(configuredValue);
    }

    unsigned int RuntimeThreadSettings::GetOpenCVThreadCount(const Configuration& config)
    {
        return GetConfiguredThreadCount(config, OpenCVThreadsConfigKey);
    }

    unsigned int RuntimeThreadSettings::GetOrtIntraThreadCount(const Configuration& config)
    {
        return GetConfiguredThreadCount(config, OrtIntraThreadsConfigKey);
    }

    unsigned int RuntimeThreadSettings::GetOrtInterThreadCount(const Configuration& config)
    {
        return GetConfiguredThreadCount(config, OrtInterThreadsConfigKey);
    }

    void RuntimeThreadSettings::ApplyThreadLimit(const Configuration& config)
    {
        const unsigned int threadCount = GetOpenCVThreadCount(config);
        if (threadCount == 0)
            return; // Default behavior, no change to OpenCV threading.
        if (threadCount == 1) 
            cv::setNumThreads(0); // OpenCV uses 0 to disable its own threading, 
        else
            cv::setNumThreads(static_cast<int>(threadCount));
    }
}
