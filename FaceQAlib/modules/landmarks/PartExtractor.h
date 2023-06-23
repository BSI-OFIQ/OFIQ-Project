#pragma once

#include "faceqa_lib.h"

namespace FACEQA_LIB::modules::landmarks
{

    enum class FaceParts;

    struct LandmarkPair
    {
        FACEQA::LandmarkPoint Upper;
        FACEQA::LandmarkPoint Lower;

        LandmarkPair(FACEQA::LandmarkPoint upper, FACEQA::LandmarkPoint lower) : Upper{upper}, Lower{lower}
        {
        }
    };

    class PartExtractor
    {
    public:
        static FACEQA::Landmarks getFacePart(const FACEQA::FaceLandmarks& faceLandmarks, FaceParts part);
        static std::vector<LandmarkPair> getPairsForPart(const FACEQA::FaceLandmarks& faceLandmarks, FaceParts part);
    };
}