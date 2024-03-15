/**
 * @file CompressionArtifacts.h
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
 * @brief Provides a class implemtenting the no compression artifact measure.
 * @author OFIQ development team
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
     * @brief Implementation of the no compression artifacts measure.
     * @details Assessment of the abscence of compression artifact
     * (both JPEG and JPEG2000) based on a CNN trained
     * by the OFIQ development team.
     */
    class CompressionArtifacts : public Measure
    {
    public:
        /**
         * @brief Constructor
         * @details The configuration parameter must contain the following entry:
         * <ul>
         *  <li><code>params.measures.CompressionArtifacts.model_path</code>: 
         *  Path to the CNN model file in ONNX format.</li>
         * </ul>
         * The following entries can (but do not need to) be configured.
         * <ul>
         *  <li><code>params.measures.CompressionArtifacts.dim</code>: If configured, the value must be 248 
         *  which corresponds to the dimension of the CNN's input layer; other values will result in an
         *  error being thrown when OFIQ's CNN is invoked.
         *  </li>
         *  <li><code>params.measures.CompressionArtifacts.crop</code>: 
         *  Top, right, bottom, and left margin by which the aligned input image will be cropped before 
         *  being scaled to the target dimension input of the CNN.</li>
         * </ul>
         * @param configuration Configuration object from which measure-related configuration is read. 
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method
         * @throws OFIQ_LIB::OFIQError if no valid model path is configured.
         */
        CompressionArtifacts(const Configuration& configuration, Session& session);

        /**
         * @brief Assesses abscence of compression artifacts.
         * @details Assessment of the abscence of compression artifact
         * (both JPEG and JPEG2000) based on a CNN trained
         * by the OFIQ development team.
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         */
        void Execute(OFIQ_LIB::Session& session) override;

    private:
        /**
         * @brief Top, right, left, and bottom margin by which the aligned image is cropped.
         * @details The value can be configured by passing a corresponding configuration to the constructor.
         * @warning The value should be 184 such that an aligned input image of dimension 616 x 616 is cropped
         * to an image of dimension 248 x 248.
         */
        uint16_t m_crop;

        /**
         * @brief Target dimension of cropped image being scaled before input to the CNN .
         * @details The cropped image is scaled to the dimension m_dim x m_dim. 
         * The value can be configured by passing a corresponding configuration to the constructor.
         * @warning The value should be 248; if configured differently, do not expect that the cropped image
         * can be successfully be passed to the CNN.
         */
        uint16_t m_dim;

        /**
         * @brief Manages CNN estimations. 
         */
        ONNXRuntimeSegmentation m_onnxRuntimeEnv;
    };
}