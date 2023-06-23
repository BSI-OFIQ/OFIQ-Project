#pragma once

#include "detectors.h"
#include "Measure.h"

namespace FACEQA_LIB::modules::measures
{
    class TotalFacesPresent : public Measure
    {
    public:
        TotalFacesPresent(
            const Configuration& configuration, Session& session, FaceDetectorInterface& detector);

        virtual void Execute() override;

    private:
        FaceDetectorInterface& detector;
    };
}