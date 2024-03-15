/**
 * @file CropOfTheFaceImage.h
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
 * @brief Provides a class implementing the crop of the face image measure.
 * @author OFIQ devlopment team
 */
#pragma once

#include "landmarks.h"
#include "Measure.h"

/**
 * @brief  Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implementation of the crop of the face image measure.
     * @details The crop of the face images measures whether the face is centered on the input image 
     * by comparing the resolution of the image with the landmarks detected during the pre-processing.
     */
    class CropOfTheFaceImage : public Measure
    {
    public:
         /**
         * @brief Constructor
         * @param configuration Configuration object from which measure-related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results 
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method 
         */
        CropOfTheFaceImage(const Configuration& configuration, Session& session);

        /**
         * @brief Implementation of the crop of the face image measure.
         * @details The crop of the face images measures whether the face is centered on the input image 
         * by comparing the resolution of the image with the landmarks detected during the pre-processing.
         * @param session Session object.
         */
        void Execute(OFIQ_LIB::Session & session) override;
    };
}