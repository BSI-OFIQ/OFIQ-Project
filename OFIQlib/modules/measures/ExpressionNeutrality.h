/**
 * @file ExpressionNeutrality.h
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
 * @brief Provides a class implementing the expression neutrality measure.
 * @author OFIQ development team
 * 
 */
#pragma once

#include "landmarks.h"
#include "Measure.h"
#include <ONNXRTSegmentation.h>

 /**
  * @brief Provides measures implemented in OFIQ.
  */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Provides a class implementing the expression neutrality measure.
     * @details The algorithm uses the CNN models enet_b0_8_best_vgaf and enet_b2_8 
     * from https://github.com/HSE-asavchenko/face-emotion-recognition as feature extractors 
     * and an AdaBoost classifier implemented in OpenCV. Inspired by Grimmer et al. [9], 
     * both CNN models have been modified to also output the embeddings of the second last layer, 
     * and have been converted to ONNX.
     */
    class ExpressionNeutrality : public Measure
    {
    public:
        /**
         * @brief Construct a new Expression Neutrality object
         * @param configuration Configuration object from which measure-related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results 
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method 
         */
        ExpressionNeutrality(
            const Configuration& configuration, Session& session);

        /**
         * @brief Run the computation based on the data passed by the session object.
         * 
         * @param session Session object
         */
        void Execute(OFIQ_LIB::Session& session) override;

    private:
        /**
         * @brief Instance of the enet_b0_8_best_vgaf_embed2 model. 
         * Set by ExpressionNeutrality.cnn1_model_path in the configuration file.
         */
        ONNXRuntimeSegmentation m_onnxRuntimeEnvCNN1;

        /**
         * @brief Instance of the enet_b2_8 model.
         * Set by ExpressionNeutrality.cnn2_model_path in the configuration file.
         */
        ONNXRuntimeSegmentation m_onnxRuntimeEnvCNN2;

        /**
         * @brief Instance of the AdaBoost classifier
         * Set by ExpressionNeutrality.adaboost_model_path in the configuration file.
         */
        std::shared_ptr<cv::ml::Boost> classifier;
    };
}