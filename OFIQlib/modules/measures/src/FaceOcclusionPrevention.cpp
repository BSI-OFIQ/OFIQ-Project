/**
 * @file FaceOcclusionPrevention.cpp
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

#include "FaceOcclusionPrevention.h"
#include "OFIQError.h"
#include "FaceMeasures.h"
#include "utils.h"
#include <opencv2/core.hpp>

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::FaceOcclusionPrevention;
    
    FaceOcclusionPrevention::FaceOcclusionPrevention(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
    }

    void FaceOcclusionPrevention::Execute(OFIQ_LIB::Session & session)
    {
        cv::Mat mask = session.getAlignedFaceLandmarkedRegion();
        int G = cv::countNonZero(mask);
        if ( G == 0)
            return SetQualityMeasure(session, qualityMeasure, 0, OFIQ::QualityMeasureReturnCode::FailureToAssess);
        
        cv::Mat faceOcclusionMask = session.getFaceOcclusionSegmentationImage();
        cv::Mat occlusionMask = mask.mul(1 - faceOcclusionMask);
        double rawScore = cv::countNonZero(occlusionMask) / (double)G;
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
