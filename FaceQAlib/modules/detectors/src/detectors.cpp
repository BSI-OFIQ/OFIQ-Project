#include "detectors.h"
#include "utils.h"

namespace FACEQA_LIB
{
    std::vector<FACEQA::BoundingBox>&
        FaceDetectorInterface::detectFaces(FACEQA_LIB::Session& session)
    {
        if (session.Id() != lastSessionId)
        {
            faces = UpdateFaces(session);
            biggestFace = findLargestBoundingBox(faces);
            session.assessment.boundingBox = faces[biggestFace];
            lastSessionId = session.Id();
        }
        return faces;
    }
}