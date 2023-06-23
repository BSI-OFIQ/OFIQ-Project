#pragma once

#include "faceqa_lib.h"
#include "Session.h"

namespace FACEQA_LIB
{
    class FaceLandmarkExtractorInterface
    {
    public:
        virtual ~FaceLandmarkExtractorInterface() {}

        FACEQA::FaceLandmarks& extractLandmarks(FACEQA_LIB::Session& session);

    protected:
        virtual FACEQA::FaceLandmarks updateLandmarks(FACEQA_LIB::Session& session) = 0;

    private:
        std::string lastSessionId;
        FACEQA::FaceLandmarks landmarks;
    };
}