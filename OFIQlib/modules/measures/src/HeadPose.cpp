/**
 * @file HeadPose.cpp
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

#include "HeadPose.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace OFIQ_LIB::modules::measures
{

    HeadPose::HeadPose(
        const Configuration& configuration, Session& session)
        : Measure{configuration, session, OFIQ::QualityMeasure::HeadPose}
    {
    }

    OFIQ::QualityMeasureResult CalculateQuality(const double& rawValue)
    {
        auto myCos = cos(rawValue * M_PI / 180);
        myCos *= myCos;
        auto scalarScore = std::max(0.0, round(100 * myCos));
        return {rawValue, scalarScore, OFIQ::QualityMeasureReturnCode::Success};
    }

    void HeadPose::Execute(OFIQ_LIB::Session & session)
    {
        auto headPose = session.getPose();

        session.assessment().qAssessments[OFIQ::QualityMeasure::HeadPoseRoll] =
            CalculateQuality(headPose[2]);
        session.assessment().qAssessments[OFIQ::QualityMeasure::HeadPosePitch] =
            CalculateQuality(headPose[0]);
        session.assessment().qAssessments[OFIQ::QualityMeasure::HeadPoseYaw] =
            CalculateQuality(headPose[1]);
    }
}