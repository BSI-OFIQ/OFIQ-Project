/**
 * @file BackgroundUniformity.h
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
 * @brief Provides a class implementing the background uniformity measure.
 * @author OFIQ development team
 */

#pragma once

#include "Measure.h"

/**
 * @brief Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implementation of the background uniformity measure.
     * @details Uniformity of the backgound is measured on basis of
     * the mean of the gradients computed on the background as per face
     * parsing (see \link OFIQ_LIB::modules::segmentations::FaceParsing \endlink).
     */
    class BackgroundUniformity : public Measure
    {
    public:
        /**
         * @brief Constructor
         * @param configuration Configuration object from which the measure related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results, 
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink.
         */
        BackgroundUniformity(
            const Configuration& configuration,
            Session& session);

        /**
         * @brief Assesses uniformity of the background.
         * @details Assessment of the background uniformity is done by computing the mean of the background
         * as per face parsing (see \link OFIQ_LIB::modules::segmentations::FaceParsing \endlink).
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing() \endlink.
         */
        void Execute(OFIQ_LIB::Session & session) override;

    private:
        /**
         * @brief The aligned image and the face parsing mask is brought to 
         * an image of the target height before gradient computations and assessment is applied.
         */
        uint16_t m_target_height = 292;

        /**
         * @brief The aligned image and the face parsing mask is brought to
         * an image of the target width before gradient computations and assessment is applied.
         */
        uint16_t m_target_width = 354;

        /**
         * @brief Crop from the left side of the aligned image 
         * (returned by \link OFIQ_LIB::Session::getAlignedFace() \endlink)
         * before scaling to the target dimension is applied.
         */
        uint16_t m_crop_left = 62;

        /**
         * @brief Crop from the right side of the aligned image
         * (returned by \link OFIQ_LIB::Session::getAlignedFace() \endlink)
         * before scaling to the target dimension is applied.
         */
        uint16_t m_crop_right = 62;

        /**
         * @brief Crop from the top of the aligned image
         * (returned by \link OFIQ_LIB::Session::getAlignedFace() \endlink)
         * before scaling to the target dimension is applied.
         */
        uint16_t m_crop_top = 0;

        /**
         * @brief Crop from the bottom of the aligned image
         * (returned by \link OFIQ_LIB::Session::getAlignedFace() \endlink)
         * before scaling to the target dimension is applied.
         */
        uint16_t m_crop_bottom = 210;

        /**
         * @brief Size of the erosion kernel applied to the background
         * as per \link OFIQ_LIB::modules::segmentations::FaceParsing \endlink
         * to reduce the risk that background unformity estimation is applied
         * to part of the subject.
         */
        uint16_t m_erosion_kernel_size = 4;
    };
}