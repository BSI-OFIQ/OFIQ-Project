/**
 * @file HeadSize.cpp
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

#include "HeadSize.h"
#include "FaceMeasures.h"
#include "utils.h"
#include <cmath>

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::HeadSize;
    
    HeadSize::HeadSize(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure}
    {
        SigmoidParameters defaultValues;
        defaultValues.setInverse();
        defaultValues.h = 200;
        defaultValues.x0 = 0.0;
        defaultValues.w = 0.05;
        defaultValues.round = true;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void HeadSize::Execute(OFIQ_LIB::Session & session)
    {
        auto faceLandmarks = session.getLandmarks();
        double T = tmetric(faceLandmarks);

        double rawScore = T / (double)session.image().height;
        double convertedScore = abs(rawScore - 0.45);

        auto scalarScore = ExecuteScalarConversion(qualityMeasure, convertedScore);
        session.assessment().qAssessments[qualityMeasure] = {rawScore, scalarScore, OFIQ::QualityMeasureReturnCode::Success};
    }
}