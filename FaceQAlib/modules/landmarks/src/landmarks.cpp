#include "landmarks.h"

namespace FACEQA_LIB
{
    FACEQA::FaceLandmarks&
        FaceLandmarkExtractorInterface::extractLandmarks(FACEQA_LIB::Session& session)
    {
        if (session.Id() != lastSessionId)
        {
            landmarks = updateLandmarks(session);
            lastSessionId = session.Id();
        }
        return landmarks;
    }
}