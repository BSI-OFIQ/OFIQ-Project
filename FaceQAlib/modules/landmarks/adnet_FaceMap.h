#pragma once

#include "FaceParts.h"
#include <array>
#include <map>
#include <vector>

// taken from https://arxiv.org/pdf/2109.05721.pdf Appendix A, Figure 6.

namespace FACEQA_LIB::modules::landmarks::adnet
{

    const LandmarkIds leftEye{60,61,62,63,64,65,66,67}; //  # left as seen on the image, it's the persons right eye
    const LandmarkIds rightEye{68,69,70,71,72,73,74,75}; //  # right as seen on the image, it's the persons left eye
    const LandmarkIds leftEyeCorners{60,64};
    const LandmarkIds rightEyeCorners{68,72};

    const LandmarkIds mouthOuter{76,77,78,79,80,81,82,83,84,85,86,87};
    
    const LandmarkIds mouthInner{88,89,90,91,92,93,94,95};
    
    const LandmarkIds contour{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    const landmarks::FaceMap FaceMap{
        {FaceParts::LEFT_EYE,          leftEye       },
        {FaceParts::RIGHT_EYE,         rightEye      },
        {FaceParts::LEFT_EYE_CORNERS,  leftEyeCorners},
        {FaceParts::RIGHT_EYE_CORNERS, rightEyeCorners},
        {FaceParts::MOUTH_OUTER,       mouthOuter    },
        {FaceParts::MOUTH_INNER,       mouthInner    },
        {FaceParts::FACE_CONTOUR,      contour       }
    };

    const LandmarkIdPairs pairsLeftEye{
        {61, 67},
        {62, 66},
        {63, 65}
    };
    const LandmarkIdPairs pairsRightEye{
        {69, 75},
        {70, 74},
        {71, 73}
    };

    const LandmarkIdPairs pairsInnerLip{
        {89, 95},
        {90, 94},
        {91, 93}
    };
    const LandmarkIdPairs pairsMouthCenter{
        {90, 94}
    };

    const landmarks::FacePairMap FacePairMap{
        {FaceParts::LEFT_EYE,     pairsLeftEye    },
        {FaceParts::RIGHT_EYE,    pairsRightEye   },
        {FaceParts::MOUTH_INNER,  pairsInnerLip   },
        {FaceParts::MOUTH_CENTER, pairsMouthCenter}
    };
}