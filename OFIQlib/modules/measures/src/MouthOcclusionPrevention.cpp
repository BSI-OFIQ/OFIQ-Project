/**
 * @file MouthOcclusionPrevention.cpp
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

#include "MouthOcclusionPrevention.h"
#include <opencv2/imgproc.hpp>


namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::MouthOcclusionPrevention;

    MouthOcclusionPrevention::MouthOcclusionPrevention(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
    }

    void MouthOcclusionPrevention::Execute(OFIQ_LIB::Session & session)
    {
        auto alignedFaceLandmarks = session.getAlignedFaceLandmarks();
        cv::Mat alignedFace = session.getAlignedFace();
        cv::Mat faceOcclusionMask = session.getFaceOcclusionSegmentationImage();

        std::vector<cv::Point2i> landmarks;
        for (int i = 76; i < 88; i++)
        {
            landmarks.push_back({ alignedFaceLandmarks.landmarks[i].x, alignedFaceLandmarks.landmarks[i].y });
        }

        cv::Mat mask = cv::Mat::zeros(alignedFace.size(), CV_8UC1);
        cv::fillConvexPoly(mask, landmarks, cv::Scalar(1));

        cv::Mat occlusionMask = mask.mul(1 - faceOcclusionMask);
        double rawScore = cv::sum(occlusionMask).val[0] / cv::sum(mask).val[0];
        double scalarScore = round(100 * (1 - rawScore));
        if (scalarScore < 0)
        {
            scalarScore = 0;
        }
        else if (scalarScore > 100)
        {
            scalarScore = 100;
        }
        session.assessment().qAssessments[qualityMeasure] = { rawScore, scalarScore, OFIQ::QualityMeasureReturnCode::Success };
    }
}