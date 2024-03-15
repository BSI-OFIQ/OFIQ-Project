/**
 * @file segmentations.h
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
 * @brief Base class for the different implementation of segmentation algorithms.
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
     * Namespace for implementations related to facial segmentations. 
     */
    namespace modules::segmentations
    {
        /**
         * @brief Enum class of the different face regioons that can be segmented.
         */
        enum class SegmentClassLabels
        {
            /**
             * @brief background label 
             */
            background,
            /**
             * @brief skin label
             */
            skin,
            /**
             * @brief left eye brow
             */
            l_brow,
            /**
             * @brief right eye brow
             */
            r_brow,
            /**
             * @brief left eye
             */
            l_eye,
            /**
             * @brief right eye brow
             */
            r_eye,
            /**
             * @brief eye glasses 
             */
            eye_g,
            /**
             * @brief left ear
             */
            l_ear,
            /**
             * @brief right eye brow
             */
            r_ear,
            /**
             * @brief earring 
             */
            ear_r,
            /**
             * @brief nose
             */
            nose,
            /**
             * @brief mouth 
             */
            mouth,
            /**
             * @brief upper lip 
             */
            u_lip,
            /**
             * @brief lower lip 
             */
            l_lip,
            /**
             * @brief neck 
             */
            neck,
            /**
             * @brief necklace 
             */
            neck_l,
            /**
             * @brief clothing 
             */
            cloth,
            /**
             * @brief hair 
             */
            hair,
            /**
             * @brief head covering
             */
            hat,
            /**
             * @brief face 
             */
            face
        };
    }

    /**
     * @brief Base class for the different implementation of segmentation algorithms.
     * @details Base class for the FaceParsing 
     * (see \link OFIQ_LIB::modules::segmentations::FaceParsing \endlink) 
     * and FaceOcclusionSegmentation 
     * (see \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation \endlink)
     */
    class SegmentationExtractorInterface
    {
    public:
        /**
         * @brief Standard destructor
         * 
         */
        virtual ~SegmentationExtractorInterface() = default;

        /**
         * @brief Get a mask of the face region requested.
         * 
         * @param session Object containing the relevant data information on the input image.
         * @param faceSegment Enum of the face region that is requested.
         * @return OFIQ::Image& Refernce on the mask of the face region image.
         */
        OFIQ::Image& GetMask(
            OFIQ_LIB::Session& session, modules::segmentations::SegmentClassLabels faceSegment);

    protected:

        /**
         * @brief Segmentation call that has to be implemented in the derived class.
         * 
         * @param session Object containing the relevant data information on the input image.
         * @param faceSegment Enum of the face region that is requested
         * @return OFIQ::Image Segmented face region mask.
         */
        virtual OFIQ::Image UpdateMask(
            OFIQ_LIB::Session& session,
            modules::segmentations::SegmentClassLabels faceSegment) = 0;

		/**
		 * @brief Accesses the last session id for this interface.
		 * @return Session id
		 */
        std::string GetLastSessionId() const { return lastSessionId; };

    private:
        /**
         * @brief id of the session that has been used in the latest request, for internal use.
         * 
         */
		std::string lastSessionId;
        /**
         * @brief Container for storing the segmented face region masks.
         * 
         */
        std::map<modules::segmentations::SegmentClassLabels, OFIQ::Image> masks;
    };
}