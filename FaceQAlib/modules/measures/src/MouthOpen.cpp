#include "MouthOpen.h"
#include "FaceMeasures.h"
#include "FaceParts.h"

namespace FACEQA_LIB::modules::measures
{

    static const auto qualityMeasure = FACEQA::QualityMeasure::MouthOpen;

    MouthOpen::MouthOpen(
        const Configuration& configuration,
        Session& session,
        FaceLandmarkExtractorInterface& landmarkExtractor)
        : Measure{configuration, session},
          landmarkExtractor{landmarkExtractor}
    {
        SigmoidParameters defaultValues;
        defaultValues.setInverse();
        defaultValues.x0 = 0.2;
        defaultValues.w = 0.06;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void MouthOpen::Execute()
    {
        auto& faceLandmarks = landmarkExtractor.extractLandmarks(session);
        auto maxMouthOpening = landmarks::FaceMeasures::GetMaxPairDistance(
            faceLandmarks,
            landmarks::FaceParts::MOUTH_INNER);
        auto interEyeDistance = landmarks::FaceMeasures::InterEyeDistance(faceLandmarks);

        auto rawScore = maxMouthOpening / interEyeDistance;

        SetQualityMeasure(qualityMeasure, rawScore);
    }


}