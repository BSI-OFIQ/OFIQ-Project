#pragma once

#include "landmarks.h"
#include "Measure.h"

namespace FACEQA_LIB::modules::measures
{
    class DynamicRange : public Measure
    {
    public:
        DynamicRange(
            const Configuration& configuration,
            Session& session,
            FaceLandmarkExtractorInterface& landmarkExtractor);

        virtual void Execute() override;

    private:
        FaceLandmarkExtractorInterface& landmarkExtractor;
    };
}