/**
 * @file DynamicRange.cpp
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

#include "DynamicRange.h"
#include "FaceMeasures.h"
#include "utils.h"
#include "image_utils.h"
#include <opencv2/imgproc.hpp>

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::DynamicRange;

    static double ComputeEntropy(const cv::Mat& luminanceImage, const cv::Mat& maskImage);
    static double CalculateScore(const cv::Mat1f& histogram);

    DynamicRange::DynamicRange(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
    }

    void DynamicRange::Execute(OFIQ_LIB::Session & session)
    {
        cv::Mat alignedImage = session.getAlignedFace();
        cv::Mat cvMask = session.getAlignedFaceLandmarkedRegion();
        cv::Mat faceSegmentation;
        cv::bitwise_and(alignedImage, alignedImage, faceSegmentation, cvMask);
        auto luminanceImage = GetLuminanceImageFromBGR(faceSegmentation);

        auto rawScore = ComputeEntropy(luminanceImage, cvMask);
        auto scalarScore = round(12.5 * rawScore);
        if (scalarScore < 0.0)
        {
            scalarScore = 0.0;
        }
        else if (scalarScore > 100.0)
        {
            scalarScore = 100.0;
        }
        session.assessment().qAssessments[qualityMeasure] = { rawScore, scalarScore, OFIQ::QualityMeasureReturnCode::Success };
    }

    static double ComputeEntropy(const cv::Mat& luminanceImage, const cv::Mat& maskImage)
    {
        int histSize = 256;
        std::vector<float> range = {0, 256};
        cv::Mat1f histogram;

        cv::calcHist(std::vector{luminanceImage}, {0}, maskImage, histogram, {histSize}, range);

        return CalculateScore(histogram);
    }

    static double CalculateScore(const cv::Mat1f& histogram)
    {
        auto pixelsInHistogram = cv::sum(histogram).val[0];
        double rawScore = 0;

        for (auto item : histogram)
        {
            auto normalizedValue = item / pixelsInHistogram;
            if (normalizedValue != 0)
            {
                rawScore += normalizedValue * log2(normalizedValue);
            }
        }

        rawScore *= -1;
        return rawScore;
    }
}