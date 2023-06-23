#pragma once

#include "faceqa_lib.h"

namespace FACEQA_LIB
{
    struct NeuronalNetworkContainer;

    struct Session
    {
        Session(const FACEQA::Image& image, FACEQA::FaceImageQualityAssessment& assessment)
            : image{image},
              assessment{assessment},
              id{GenerateId()}
        {
        }

        const FACEQA::Image& image;
        FACEQA::FaceImageQualityAssessment& assessment;

        const std::string& Id() { return id; }

    private:
        std::string GenerateId();
        std::string id;
    };
}