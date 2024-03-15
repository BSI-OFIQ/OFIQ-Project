/**
 * @file EyesVisible.cpp
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

#include "EyesVisible.h"
#include "OFIQError.h"
#include "FaceMeasures.h"
#include "FaceParts.h"
#include <opencv2/imgproc.hpp>

using PartExtractor = OFIQ_LIB::modules::landmarks::PartExtractor;
using FaceParts = OFIQ_LIB::modules::landmarks::FaceParts;

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::EyesVisible;

    EyesVisible::EyesVisible(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
    }

    void EyesVisible::Execute(OFIQ_LIB::Session & session)
    {
        auto alignedFaceLandmarks = session.getAlignedFaceLandmarks();
        cv::Mat faceOcclusionMask = session.getFaceOcclusionSegmentationImage();
        OFIQ::Landmarks leftEye = PartExtractor::getFacePart(alignedFaceLandmarks, FaceParts::LEFT_EYE);
        OFIQ::Landmarks rightEye = PartExtractor::getFacePart(alignedFaceLandmarks, FaceParts::RIGHT_EYE);

        auto headPose = session.getPose();
        auto interEyeDistance = landmarks::FaceMeasures::InterEyeDistance(alignedFaceLandmarks, headPose[1]);

        // Determine EVZ according to ISO/IEC 39794-5
        int V = interEyeDistance / 20;
        std::vector<cv::Point2i> leftEyePoints;
        for (auto landmark : leftEye)
        {
            cv::Point point{ landmark.x, landmark.y };
            leftEyePoints.push_back(point);
        }
        cv::Rect leftBoundingRect = cv::boundingRect(leftEyePoints);
        std::vector<cv::Point2i> leftRect = { 
            cv::Point2i{leftBoundingRect.x - V, leftBoundingRect.y - V},
            cv::Point2i{leftBoundingRect.x + leftBoundingRect.width + V, leftBoundingRect.y - V},
            cv::Point2i{leftBoundingRect.x + leftBoundingRect.width + V, leftBoundingRect.y + leftBoundingRect.height + V},
            cv::Point2i{leftBoundingRect.x - V, leftBoundingRect.y + leftBoundingRect.height + V}
        };

        std::vector<cv::Point2i> rightEyePoints;
        for (auto landmark: rightEye)
        {
            cv::Point point{ landmark.x, landmark.y };
            rightEyePoints.push_back(point);
        }
        cv::Rect rightBoundingRect = cv::boundingRect(rightEyePoints);
        std::vector<cv::Point2i> rightRect = {
            cv::Point2i{rightBoundingRect.x - V, rightBoundingRect.y - V},
            cv::Point2i{rightBoundingRect.x + rightBoundingRect.width + V, rightBoundingRect.y - V},
            cv::Point2i{rightBoundingRect.x + rightBoundingRect.width + V, rightBoundingRect.y + rightBoundingRect.height + V},
            cv::Point2i{rightBoundingRect.x - V, rightBoundingRect.y + rightBoundingRect.height + V}
        };

        std::vector<std::vector<cv::Point2i>> contours = { leftRect, rightRect };
        cv::Mat EVZMask = cv::Mat::zeros(faceOcclusionMask.size(), CV_8U);
        cv::drawContours(EVZMask, contours, -1, 1, -1);
        
        // Compute proportion of occlusion of EVZ
        cv::Mat occlusionMask = EVZMask.mul(1- faceOcclusionMask);
        double rawScore = cv::sum(occlusionMask).val[0] / cv::sum(EVZMask).val[0];
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