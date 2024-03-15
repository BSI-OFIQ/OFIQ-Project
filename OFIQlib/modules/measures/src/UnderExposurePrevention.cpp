/**
 * @file UnderExposurePrevention.cpp
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

#include "UnderExposurePrevention.h"
#include "FaceMeasures.h"
#include "utils.h"
#include <opencv2/imgproc.hpp>
#include "OFIQError.h"
#include "image_utils.h"

namespace OFIQ_LIB::modules::measures
{
    /// @brief range of exposure values, borders are included {minValue, maxValue}
    
    static const ExposureRange darkRange = {0, 25};
    
    static const auto qualityMeasure = OFIQ::QualityMeasure::UnderExposurePrevention;

    static double ComputeBrightnessAspect(
        const cv::Mat& luminanceImage,
        const cv::Mat& maskImage,
        const ExposureRange& exposureRange);
    static double CalculateScore(const cv::Mat1f& histogram, const ExposureRange& exposureRange);
    
    UnderExposurePrevention::UnderExposurePrevention(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
        SigmoidParameters defaultValues;
        defaultValues.h = 120;
        defaultValues.a = 0.832;
        defaultValues.s = -1;
        defaultValues.x0 = 0.92;
        defaultValues.w = 0.05;
        defaultValues.round = true;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void UnderExposurePrevention::Execute(OFIQ_LIB::Session & session)
    {
        double rawScore = CalculateExposure(session, darkRange);
        SetQualityMeasure(session, qualityMeasure, rawScore, OFIQ::QualityMeasureReturnCode::Success);
    }
}