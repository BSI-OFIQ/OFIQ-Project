#pragma once

#include "landmarks.h"
#include "Measure.h"

namespace FACEQA_LIB::modules::measures
{
    class MouthOpen : public Measure
    {
    public:
        MouthOpen(
            const Configuration& configuration,
            Session& session,
            FaceLandmarkExtractorInterface& landmarkExtractor);

        virtual void Execute() override;

    private:
        FaceLandmarkExtractorInterface& landmarkExtractor;
    };
}