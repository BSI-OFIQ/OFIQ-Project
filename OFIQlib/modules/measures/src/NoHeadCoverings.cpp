/**
 * @file NoHeadCoverings.cpp
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

#include "NoHeadCoverings.h"
#include "segmentations.h"
#include <opencv2/imgproc.hpp>

namespace OFIQ_LIB::modules::measures
{
    using Segment = OFIQ_LIB::modules::segmentations::SegmentClassLabels;

    static const auto qualityMeasure = OFIQ::QualityMeasure::NoHeadCoverings;
    static const std::string paramThreshold = "params.measures.NoHeadCoverings.threshold";

    NoHeadCoverings::NoHeadCoverings(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
        if (!configuration.GetNumber(paramThreshold, this->threshold))
            this->threshold = 0.02;
    }

    void NoHeadCoverings::Execute(OFIQ_LIB::Session & session)
    {
        cv::Mat M = session.getFaceParsingImage();

        // Crop M from the bottom by 204 pixels
        cv::Rect rect(0, 0, M.cols, M.rows - 204);
        cv::Mat croppedImage = M(rect);

        // Count the number n of pixels in M having value 16 or 18
        cv::Mat mask;
        cv::Mat clothMask;
        cv::Mat hatMask;
        // set all values above and below Segment::cloth to 0
        cv::threshold(croppedImage, mask, static_cast<uchar>(Segment::cloth) - 1, 255, cv::THRESH_TOZERO);
        cv::threshold(mask, clothMask, static_cast<uchar>(Segment::cloth), 255, cv::THRESH_TOZERO_INV);
        // set all values above and below Segment::hat to 0
        cv::threshold(mask, hatMask, static_cast<uchar>(Segment::hat) - 1, 255, cv::THRESH_TOZERO);
        cv::threshold(hatMask, hatMask, static_cast<uchar>(Segment::hat), 255, cv::THRESH_TOZERO_INV);
        auto clothPixels = cv::countNonZero(clothMask);
        auto hatPixels = cv::countNonZero(hatMask);

        // Output n/m where m is the number of pixels in M
        auto nonZeroPixels = clothPixels + hatPixels;
        auto totalPixels = croppedImage.size().area();
        double rawScore = nonZeroPixels / (double)totalPixels;

        double scalarScore;
        if (rawScore > this->threshold)
        {
            scalarScore = 0;
        }
        else
        {
            scalarScore = 100;
        }

        session.assessment().qAssessments[qualityMeasure] = { rawScore, scalarScore, OFIQ::QualityMeasureReturnCode::Success };
    }
}