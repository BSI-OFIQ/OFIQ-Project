#include "TotalFacesPresent.h"
#include "FaceQaError.h"

namespace FACEQA_LIB::modules::measures
{
    FACEQA::QualityMeasure qualityMeasure = FACEQA::QualityMeasure::TotalFacesPresent;

    TotalFacesPresent::TotalFacesPresent(
        const Configuration& configuration, Session& session, FaceDetectorInterface& detector)
        : Measure{configuration, session},
          detector{detector}
    {
    }

    void TotalFacesPresent::Execute()
    {
        auto faces = detector.detectFaces(session);
        auto rawValue = faces.size();
        double scalarValue = rawValue == 1 ? 100 : 0;

        session.assessment.qAssessments[qualityMeasure] = {
            static_cast<double>(rawValue),
            scalarValue};

        if (faces.empty())
            throw FaceQaError(
                FACEQA::ReturnCode::FaceDetectionError,
                "There were no faces detected");
    }
}