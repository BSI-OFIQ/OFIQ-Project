/**
 * @file Measure.cpp
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

#include "Measure.h"

#include <magic_enum.hpp>

namespace OFIQ_LIB::modules::measures
{
    void Measure::AddSigmoid(OFIQ::QualityMeasure measure, const SigmoidParameters& defaultValues)
    {
        AddSigmoid(GetMeasureName(measure), defaultValues);
    }

    void Measure::AddSigmoid(const std::string& key, SigmoidParameters sigmoidParams)
    {
        auto extendedKey = ExpandKey(key);
        configuration.GetNumber(extendedKey + "h", sigmoidParams.h);
        configuration.GetNumber(extendedKey + "a", sigmoidParams.a);
        configuration.GetNumber(extendedKey + "s", sigmoidParams.s);
        configuration.GetNumber(extendedKey + "x0", sigmoidParams.x0);
        configuration.GetNumber(extendedKey + "w", sigmoidParams.w);
        configuration.GetBool(extendedKey + "round", sigmoidParams.round);
        sigmoidMap[key] = sigmoidParams;
    }

    double Measure::ExecuteScalarConversion(OFIQ::QualityMeasure measure, double rawValue)
    {
        return ExecuteScalarConversion(GetMeasureName(measure), rawValue);
    }

    double Measure::ExecuteScalarConversion(const std::string& key, double rawValue)
    {
        return ScalarConversion(rawValue, sigmoidMap[key]);
    }

    void Measure::SetQualityMeasure(OFIQ_LIB::Session& session, OFIQ::QualityMeasure measure, double rawScore, OFIQ::QualityMeasureReturnCode code)
    {
        auto scalarScore = ExecuteScalarConversion(measure, rawScore);
        session.assessment().qAssessments[measure] = {rawScore, scalarScore, code};
    }

    std::string Measure::GetName() const
    {
        return Measure::GetMeasureName(this->m_measure);
    }

    std::string Measure::GetMeasureName(OFIQ::QualityMeasure measure)
    {
        return static_cast<std::string>(magic_enum::enum_name(measure));
    }

    OFIQ::QualityMeasure Measure::GetQualityMeasure() const
    {
        return m_measure;
    }

    std::string Measure::ExpandKey(std::string_view rawKey)
    {
        std::string key = "params.measures.";
        key += rawKey;
        key += ".Sigmoid.";
        return key;
    }
}
