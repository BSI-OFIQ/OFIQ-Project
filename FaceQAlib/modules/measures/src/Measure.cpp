#include "Measure.h"

#include <magic_enum.hpp>

namespace FACEQA_LIB::modules::measures
{
    void Measure::AddSigmoid(FACEQA::QualityMeasure measure, const SigmoidParameters& defaultValues)
    {
        AddSigmoid(GetKey(measure), defaultValues);
    }

    void Measure::AddSigmoid(const std::string& key, SigmoidParameters sigmoidParams)
    {
        auto extendedKey = ExpandKey(key);
        configuration.GetNumber(extendedKey+"h", sigmoidParams.h);
        configuration.GetNumber(extendedKey+"a", sigmoidParams.a);
        configuration.GetNumber(extendedKey+"s", sigmoidParams.s);
        configuration.GetNumber(extendedKey+"x0", sigmoidParams.x0);
        configuration.GetNumber(extendedKey+"w", sigmoidParams.w);
        configuration.GetBool(extendedKey+"round", sigmoidParams.round);
        sigmoidMap[key] = sigmoidParams;
    }

    double Measure::ExecuteScalarConversion(FACEQA::QualityMeasure measure, double rawValue)
    {
        return ExecuteScalarConversion(GetKey(measure), rawValue);
    }

    double Measure::ExecuteScalarConversion(const std::string& key, double rawValue)
    {
        return ScalarConversion(rawValue, sigmoidMap[key]);
    }

    void Measure::SetQualityMeasure(FACEQA::QualityMeasure measure, double rawScore)
    {
        auto scalarScore = ExecuteScalarConversion(measure, rawScore);
        session.assessment.qAssessments[measure] = {rawScore, scalarScore};
    }

    std::string Measure::GetKey(FACEQA::QualityMeasure measure)
    {
        return static_cast<std::string>(magic_enum::enum_name(measure));
    }

    std::string Measure::ExpandKey(std::string_view rawKey)
    {
        std::string key = "parameters.";
        key += rawKey;
        key += ".Sigmoid.";
        return key;
    }
}