/**
 * @file adnet_FaceMap.h
 *
 * @copyright Copyright (c) 2024  Federal Office for Information Security, Germany
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @brief Provides definitions of landmark indices to access specific
 * parts of ADNet landmarks.
 * 
 * @details The definitions provided by this header were taken from 
 * https://arxiv.org/pdf/2109.05721.pdf Appendix A, Figure 6.
 * 
 * @author OFIQ development team
 */

#pragma once

#include "FaceParts.h"
#include <array>
#include <map>
#include <vector>

/**
 * @brief Namespace for ADNet-specific landmarks 
 */
namespace OFIQ_LIB::modules::landmarks::adnet
{
    /**
     * @brief Landmark indices (ADNet) of the left eye.
     * @details The left eye is defined as seen on the image; it is actually the person's right eye (physically).
     */
    const LandmarkIds leftEye{60,61,62,63,64,65,66,67};

    /**
     * @brief Landmark indices (ADNet) of the right eye.
     * @details The right eye is defined as seen on the image; it is actually the person's left eye (physically).
     */
    const LandmarkIds rightEye{68,69,70,71,72,73,74,75};

    /**
     * @brief Landmark indices (ADNet) of the left eyes' corners. 
     */
    const LandmarkIds leftEyeCorners{60,64};

    /**
     * @brief Landmark indices (ADNet) of the right eyes' corners.
     */
    const LandmarkIds rightEyeCorners{68,72};

    /**
     *  @brief Landmark index (ADNet) of the nose tip.
     */
    const LandmarkIds nosetip{54};

    /**
     * @brief Landmark indices (ADNet) on the mouth's outer contour. 
     */
    const LandmarkIds mouthOuter{76,77,78,79,80,81,82,83,84,85,86,87};
    
    /**
     * @brief Landmark indices (ADNet) on the mouth's inner lip borders.
     */
    const LandmarkIds mouthInner{88,89,90,91,92,93,94,95};
    
    /**
     * @brief Landmark indices (ADNet) of the face contour. 
     */
    const LandmarkIds contour{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    /**
     * @brief Landmark indices (ADNet) of the forehead (empty for ADNet).
     */
    const LandmarkIds forehead{};

    /**
     * @brief Landmark index (ADNet) of the chin. 
     */
    const LandmarkIds chin{16};

    /**
     * @brief ADNets face map definition. 
     */
    const landmarks::FaceMap FaceMap{
        {FaceParts::LEFT_EYE,          leftEye       },
        {FaceParts::RIGHT_EYE,         rightEye      },
        {FaceParts::LEFT_EYE_CORNERS,  leftEyeCorners},
        {FaceParts::RIGHT_EYE_CORNERS, rightEyeCorners},
        {FaceParts::MOUTH_OUTER,       mouthOuter    },
        {FaceParts::MOUTH_INNER,       mouthInner    },
        {FaceParts::FACE_CONTOUR,      contour       },
        {FaceParts::CHIN,              chin          },
        {FaceParts::NOSETIP,           nosetip       },
        {FaceParts::FOREHEAD,          forehead      }
    };

    /**
     * @brief Pair indices of landmarks (ADNet) for the left eye.
     * @details Useful to measure eye openess.
     */
    const LandmarkIdPairs pairsLeftEye{
        {61, 67},
        {62, 66},
        {63, 65}
    };

    /**
     * @brief Landmark index pairs (ADNet) of landmarks for the right eye.
     * @details Useful to measure eye openess.
     */
    const LandmarkIdPairs pairsRightEye{
        {69, 75},
        {70, 74},
        {71, 73}
    };

    /**
     * @brief Landmark index pairs (ADNet) of inner lip pairs.
     * @details Useful to measure closedness of mouth.
     */
    const LandmarkIdPairs pairsInnerLip{
        {89, 95},
        {90, 94},
        {91, 93}
    };

    /**
     * @brief Landmark index pair (ADNet) of the inner mouth (lips) center. 
     * @details Useful to measure closedness of mouth.
     */
    const LandmarkIdPairs pairsMouthCenter{
        {90, 94}
    };

    /**
     * @brief ADNets face pair map definition.
     */
    const landmarks::FacePairMap FacePairMap{
        {FaceParts::LEFT_EYE,     pairsLeftEye    },
        {FaceParts::RIGHT_EYE,    pairsRightEye   },
        {FaceParts::MOUTH_INNER,  pairsInnerLip   },
        {FaceParts::MOUTH_CENTER, pairsMouthCenter}
    };
}