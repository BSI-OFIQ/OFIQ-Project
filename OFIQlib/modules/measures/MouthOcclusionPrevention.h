/**
 * @file MouthOcclusionPrevention.h
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
 * @brief Provides a class implementing the mouth occlusion prevention measure.
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
     * @brief Implementation of the mouth occlusion prevention measure.
     * @details Absence of mouth occlusion is assessed by measuring the coverage 
     * of the mouth region from mouth landmarks with the result of 
     * face occlusion segmentation computed on pre-processing.
     */
    class MouthOcclusionPrevention : public Measure
    {
    public:
        /**
         * @brief Constructor
         * @param configuration Configuration object from which measure-related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method
         */
        MouthOcclusionPrevention(const Configuration& configuration, Session& session);

        /**
         * @brief Assesses absence of mouth occlusion.
         * @details Absence of mouth occlusion is assessed by measuring the coverage 
         * of the mouth region from mouth landmarks with the result of 
         * face occlusion segmentation computed on pre-processing. Pre-processing results are passed to the method
         * with the <code>session</code> parameter.
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         * @see \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation FaceOcclusionSegmentation\endlink
         * @see \link OFIQ_LIB::Session::getAlignedFaceLandmarks() Session::getAlignedFaceLandmarks()\endlink
         */
        void Execute(OFIQ_LIB::Session & session) override;
    };
}