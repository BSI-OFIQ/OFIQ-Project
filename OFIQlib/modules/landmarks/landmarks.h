/**
 * @file landmarks.h
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
 * @brief Provides the base class for the implementation of face landmark extractors.
 * @author OFIQ development team
 */
#pragma once

#include "ofiq_lib.h"
#include "Session.h"

 /**
  * Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB
{
    /**
     * @brief Implements the base class for the face landmark extractors.
     * 
     */
    class FaceLandmarkExtractorInterface
    {
    public:
        /**
         * @brief Destructor
         * 
         */
        virtual ~FaceLandmarkExtractorInterface() = default;

        /**
         * @brief Public method to extract landmarks from the image passed in the session object.
         * 
         * @param session Data container, including the original image and  preprocessed data.
         * @return OFIQ::FaceLandmarks 
         */
        OFIQ::FaceLandmarks extractLandmarks(OFIQ_LIB::Session& session);
#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC //deprecated but required by DIS of ISO/IEC 29794-5
        std::vector<OFIQ::FaceLandmarks> extractLandmarksAllFaces(OFIQ_LIB::Session& session, const std::vector<OFIQ::BoundingBox>& faces );
#endif

    protected:
        /**
         * @brief Internal implementation of the derived class for extracting landmarks.
         * 
         * @param session Data container, including the original image and  preprocessed data.
         * @return OFIQ::FaceLandmarks 
         */
        virtual OFIQ::FaceLandmarks updateLandmarks(OFIQ_LIB::Session& session) = 0;
#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC //deprecated but required by DIS of ISO/IEC 29794-5
        virtual std::vector<OFIQ::FaceLandmarks> updateLandmarksAllFaces(
            OFIQ_LIB::Session& session, const std::vector<OFIQ::BoundingBox>& faces) = 0;
#endif
    };
}