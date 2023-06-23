#pragma once

#include "faceqa_lib.h"
#include "Session.h"

namespace FACEQA_LIB
{

    class FaceDetectorInterface
    {
    public:
        virtual ~FaceDetectorInterface() {}

        /**
         * @brief This function detects faces in given image.
         *
         * @param[in] session
         * Session containing relevant information for the current task.
         */
        std::vector<FACEQA::BoundingBox>& detectFaces(FACEQA_LIB::Session& session);

        size_t biggestFaceId() { return biggestFace; }

    protected:
        virtual std::vector<FACEQA::BoundingBox> UpdateFaces(FACEQA_LIB::Session& session) = 0;

    private:
        std::string lastSessionId;
        std::vector<FACEQA::BoundingBox> faces;
        size_t biggestFace;
    };
}
