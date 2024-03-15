/**
 * @file UnifiedQualityScore.h
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
 * @brief Provides a class implemtenting the unified quality measure.
 * @author OFIQ development team
 */
#pragma once

#include "landmarks.h"
#include "Measure.h"
#include <opencv2/dnn.hpp>
#include <ONNXRTSegmentation.h>

/**
 * @brief Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implementation of the unified quality measure.
     * @details The quality score refers to the requirements 
     * in clause 5.4.8 in ISO/IEC 19794-5:2011 frontal image type 
     * (relevant for EU-EES implementing decision 2019/329) 
     * and in clause 7.7 in ISO/IEC 39794-1 (relevant for UC1 specified in ICAO Document 9303)
     */
    class UnifiedQualityScore : public Measure
    {
    public:
        /**
         * @brief Construct a new Unified Quality Score object
         * @param configuration Configuration object from which measure-related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results 
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method 
         */
        UnifiedQualityScore(
            const Configuration& configuration,
            Session& session);

        /**
         * @brief Run the computation on the measure
         * @details The algorithm uses a iResNet50 model M from https://github.com/IrvingMeng/MagFace 
         * trained on MS1MV2 with MagFace loss without DDP parallelisation. 
         * The algorithm takes as input the image I output by the alignment algorithm. 
         * 
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method.
         */
        void Execute(OFIQ_LIB::Session & session) override;

    private:
        /**
         * @brief Instance of the neural network (iResNet50 model M).
         * 
         */
        ONNXRuntimeSegmentation m_onnxRuntimeEnv;
    };
}