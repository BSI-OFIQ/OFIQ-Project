/**
 * @file CropOfTheFaceImage.cpp
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

#include "CropOfTheFaceImage.h"
#include "FaceMeasures.h"
#include "FaceParts.h"
#include "utils.h"

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityDown = OFIQ::QualityMeasure::DownwardCropOfTheFaceImage;
    static const auto qualityLeft = OFIQ::QualityMeasure::LeftwardCropOfTheFaceImage;
    static const auto qualityRight = OFIQ::QualityMeasure::RightwardCropOfTheFaceImage;
    static const auto qualityUp = OFIQ::QualityMeasure::UpwardCropOfTheFaceImage;

    CropOfTheFaceImage::CropOfTheFaceImage(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, OFIQ::QualityMeasure::CropOfTheFaceImage }
    {
        SigmoidParameters defaultValues;
        defaultValues.h = 100;
        defaultValues.x0 = 0.9;
        defaultValues.w = 0.1;
        defaultValues.round = true;
        AddSigmoid(qualityLeft, defaultValues);
        AddSigmoid(qualityRight, defaultValues);

        defaultValues.Reset();
        defaultValues.h = 100;
        defaultValues.x0 = 1.8;
        defaultValues.w = 0.1;
        defaultValues.round = true;
        AddSigmoid(qualityUp, defaultValues);

        defaultValues.Reset();
        defaultValues.h = 100;
        defaultValues.x0 = 1.4;
        defaultValues.w = 0.1;
        defaultValues.round = true;
        AddSigmoid(qualityDown, defaultValues);
    }

    void CropOfTheFaceImage::Execute(OFIQ_LIB::Session & session)
    {
        auto faceLandmarks = session.getLandmarks();
        auto leftEyePoints = landmarks::PartExtractor::getFacePart(
            faceLandmarks,
            landmarks::FaceParts::LEFT_EYE_CORNERS);
        auto rightEyePoints = landmarks::PartExtractor::getFacePart(
            faceLandmarks,
            landmarks::FaceParts::RIGHT_EYE_CORNERS);

        auto leftEyeCenter = landmarks::FaceMeasures::GetMiddle(leftEyePoints);
        auto rightEyeCenter = landmarks::FaceMeasures::GetMiddle(rightEyePoints);

        auto eyeMidPoint =
            landmarks::FaceMeasures::GetMiddle(OFIQ::Landmarks{ leftEyeCenter, rightEyeCenter });

        double interEyeDistance = landmarks::FaceMeasures::GetDistance(leftEyeCenter, rightEyeCenter);

        double rawScoreLeft = rightEyeCenter.x / interEyeDistance;
        SetQualityMeasure(session, qualityLeft, rawScoreLeft, OFIQ::QualityMeasureReturnCode::Success);

        double rawScoreRight = (session.image().width - leftEyeCenter.x) / interEyeDistance;
        SetQualityMeasure(session, qualityRight, rawScoreRight, OFIQ::QualityMeasureReturnCode::Success);

        double t = tmetric(faceLandmarks);
        double rawScoreUp = (session.image().height - eyeMidPoint.y) / t;
        SetQualityMeasure(session, qualityUp, rawScoreUp, OFIQ::QualityMeasureReturnCode::Success);

        double rawScoreDown = eyeMidPoint.y / t;
        SetQualityMeasure(session, qualityDown, rawScoreDown, OFIQ::QualityMeasureReturnCode::Success);
    }
}