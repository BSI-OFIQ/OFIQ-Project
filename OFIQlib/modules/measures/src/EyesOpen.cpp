/**
 * @file EyesOpen.cpp
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

#include "EyesOpen.h"
#include "FaceMeasures.h"
#include "FaceParts.h"
#include "utils.h"

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::EyesOpen;

    EyesOpen::EyesOpen(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
        SigmoidParameters defaultValues;
        defaultValues.h = 100;
        defaultValues.x0 = 0.02;
        defaultValues.w = 0.01;
        defaultValues.round = true;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void EyesOpen::Execute(OFIQ_LIB::Session & session)
    {
        auto landmarks = session.getAlignedFaceLandmarks();
        auto leftMaxOpening =
            landmarks::FaceMeasures::GetMaxPairDistance(landmarks, landmarks::FaceParts::LEFT_EYE);
        auto rightMaxOpening =
            landmarks::FaceMeasures::GetMaxPairDistance(landmarks, landmarks::FaceParts::RIGHT_EYE);
        auto smallerEyeOpening = std::min(leftMaxOpening, rightMaxOpening);
        auto t = tmetric(landmarks);
        auto rawScore = smallerEyeOpening / t;
        SetQualityMeasure(session, qualityMeasure, rawScore, OFIQ::QualityMeasureReturnCode::Success);
    }
}