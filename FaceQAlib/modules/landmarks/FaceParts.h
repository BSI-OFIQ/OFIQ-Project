#pragma once

namespace FACEQA_LIB::modules::landmarks
{
    enum class FaceParts
    {
        LEFT_EYE,          // left as seen on the image, it's the persons right eye
        RIGHT_EYE,         // right as seen on the image, it's the persons left eye
        LEFT_EYE_CORNERS,  // two landmarks - outer, inner corner
        RIGHT_EYE_CORNERS, // two landmarks - outer, inner corner
        MOUTH_OUTER,
        MOUTH_INNER,
        FACE_CONTOUR,
        MOUTH_CENTER
    };

    using LandmarkId = int;
    using LandmarkIds = std::vector<LandmarkId>;
    using FaceMap = std::map<FaceParts, LandmarkIds>;
    using LandmarkIdPair = std::array<LandmarkId, 2>;
    using LandmarkIdPairs = std::vector<LandmarkIdPair>;
    using FacePairMap = std::map<FaceParts, LandmarkIdPairs>;
}