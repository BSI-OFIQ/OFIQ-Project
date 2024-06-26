/**
 * @file SingleFacePresent.h
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
 * @brief Provides a class implementing the single face present measure.
 * @author OFIQ development team
 */
#pragma once

#include "detectors.h"
#include "Measure.h"

/**
 * @brief Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implementation of the single face present measure.
     * @details Single face present measure shall ensure that only one face is visible on the image.
     */
    class SingleFacePresent : public Measure
    {
    public:
        /**
         * @brief Construct a new Single Face Present object
         * @param configuration Configuration object from which measure-related configuration is read.
         */
        explicit SingleFacePresent(const Configuration& configuration);

        /**
         * @brief Run computation of the single face present analysis.
         * @param session Session object containing the original facial image and pre-processing results 
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method 
         */
        void Execute(OFIQ_LIB::Session & session) override;
    };
}