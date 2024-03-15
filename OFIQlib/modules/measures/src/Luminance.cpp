/**
 * @file Luminance.cpp
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

#include "Luminance.h"
#include "FaceMeasures.h"
#include "image_utils.h"
#include "utils.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace OFIQ_LIB::modules::measures
{
    Luminance::Luminance(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, OFIQ::QualityMeasure::Luminance }
    {
    }

    void Luminance::Execute(OFIQ_LIB::Session & session)
    {
        cv::Mat aligned = session.getAlignedFace();

        // Get landmarked region segmentation map
        auto landmarks = session.getLandmarks();
        auto mask = landmarks::FaceMeasures::GetFaceMask(session.getAlignedFaceLandmarks(), aligned.rows, aligned.cols);

        // Recover the image luminance from RGB data of image
        auto luminanceImage = GetLuminanceImageFromBGR(aligned);

        // Compute the luminance histogram
        cv::Mat1f histogram;
        GetNormalizedHistogram(luminanceImage, mask, histogram);

        // Compute the mean of the luminance histogram
        double mean = 0;
        std::vector<int> range = { 0, 256 };
        for (int i = range[0]; i < range[1]; i++)
        {
            mean += histogram.at<float>(i) * i / 255.0;
        }

        double scalarScoreMean = round(100 * Sigmoid(mean, 0.2, 0.05) * (1 - Sigmoid(mean, 0.8, 0.05)));
        session.assessment().qAssessments[OFIQ::QualityMeasure::LuminanceMean] = { mean, scalarScoreMean, OFIQ::QualityMeasureReturnCode::Success };

        // Compute the variance of the luminance histogram
        double variance = 0;
        for (int i = range[0]; i < range[1]; i++)
        {
            variance += histogram.at<float>(i) * std::pow(i / 255.0 - mean, 2);
        }

        double scalarScoreVariance = round(100 * sin((60 * variance) / (60 * variance + 1) * M_PI));
        session.assessment().qAssessments[OFIQ::QualityMeasure::LuminanceVariance] = { variance, scalarScoreVariance, OFIQ::QualityMeasureReturnCode::Success };
    }
}