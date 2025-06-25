/**
 * @file ofiq_lib_impl.h
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
 * @brief Implementation of the OFIQ_LIB.
 * @author OFIQ development team
 */
#ifndef OFIQ_LIB_IMPL_H
#define OFIQ_LIB_IMPL_H

#include "Configuration.h"
#include "Executor.h"
#include "ofiq_lib.h"
#include "NeuronalNetworkContainer.h"

 /**
  * @brief Namespace for OFIQ implementations.
  */
namespace OFIQ_LIB
{
    /**
     * @brief Implementation of the OFIQ_LIB.
     * 
     */
    class OFIQImpl : public OFIQ::Interface
    {
    public:
        /**
         * @brief Constructor
         * 
         */
        OFIQImpl() = default;

        /**
         * @brief Destructor
         * 
         */
        ~OFIQImpl() override = default;

        /**
         * @brief Initialize the lib by reading the configuration file.
         * 
         * @param configDir Path to the configuration file.
         * @param configValue Name of the configuration file.
         * @return OFIQ::ReturnStatus 
         */
        OFIQ::ReturnStatus
            initialize(const std::string& configDir, const std::string& configValue) override;

        /**
         * @brief Compute an overall quality score for the image provided.
         * @details The overall quality score will be equal to the measure ualityMeasure::UnifiedQualityScore if it is activated. 
         * Otherwise, the overall quality score will be the mean of all active measure scores.
         * @param[in] face Input image.
         * @param[out] quality Computed UnifiedQualityScore.
         * @return OFIQ::ReturnStatus 
         */
        OFIQ::ReturnStatus scalarQuality(const OFIQ::Image& face, double& quality) override;

        /**
         * @brief Run the computation of all measures set in the configuration.
         * 
         * @param[in] image Input image.
         * @param[out] assessments Container to store the resulting scores.
         * @return OFIQ::ReturnStatus 
         */
        OFIQ::ReturnStatus vectorQuality(
            const OFIQ::Image& image, OFIQ::FaceImageQualityAssessment& assessments) override;

        /**
         * @brief Run the computation of all measures set in the configuration 
         * and access pre-precessing result.
         *
         * @param[in] image Input image.
         * @param[out] assessments Container to store the resulting scores.
         * @param[out] preprocessingResult Container to store preprocessing results.
         * @param[in] resultRequestsMask
         * Mask encoding the pre-processing data being requested.
         * @return OFIQ::ReturnStatus
         * 
         * @see \link OFIQ::PreprocessingRequest PreprocessingRequest\endlink
         */
        OFIQ::ReturnStatus vectorQualityWithPreprocessingResults(
            const OFIQ::Image& image,
            OFIQ::FaceImageQualityAssessment& assessments,
            OFIQ::FaceImageQualityPreprocessingResult& preprocessingResult,
            uint32_t resultRequestsMask = static_cast<int>(OFIQ::PreprocessingResultType::All)) override;

    private:
        /**
         * @brief Pointer to the executor instance, see \link OFIQ_LIB::modules::measures::Executor \endlink.
         * 
         */
        std::unique_ptr<OFIQ_LIB::modules::measures::Executor> m_executorPtr;

        /**
         * @brief Pointer to the cinfiguration
         * 
         */
        std::unique_ptr<Configuration> config;

        /**
         * @brief Pointer to the different neural network instances, used during the preprocesing.
         * 
         */
        std::unique_ptr<NeuronalNetworkContainer> networks;

        /**
         * @brief Create a Executor object
         * 
         * @return std::unique_ptr<OFIQ_LIB::modules::measures::Executor> 
         */
        std::unique_ptr<OFIQ_LIB::modules::measures::Executor> CreateExecutor();
        
        /**
         * @brief Create a NeuronalNetworkContainer
         * 
         */
        void CreateNetworks();

        /**
         * @brief Perform the preprocessing.
         * 
         * @param session Session object containing the original facial image
         * for which the preprocessing will be performed. 
         * The pre-processing results will be stored in the passed Session object.
         */
        OFIQ::ReturnStatus preprocess(Session& session);
        
        /**
         * @brief Perform the assessment.
         *
         * @param session Session object containing the original facial image 
         * and pre-processing results computed by the \link OFIQ_LIB::OFIQImpl::preprocess()
         * OFIQImpl::preprocess()\endlink method
         */
        OFIQ::ReturnStatus performAssessment(Session& session);

        /**
         * @brief Perform the face alignment.
         * 
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method
         */
        void alignFaceImage(Session& session) const;

        /**
         * @brief Processes and image and outputs its quality assessment; optionally, 
         * if requested, pre-processing data can be output by the function.
         * @param[in] image Face image
         * @param[out] assessments Structure in which the assessment is stored
         * @param[out] preprocessingResult Structure in which requested pre-processing data is stored
         * @param[in] resultRequestsMask Mask encoding the requested pre-processing results
         * @see \link OFIQ::PreprocessingRequest PreprocessingRequest\endlink
         */
        OFIQ::ReturnStatus getPreprocessingResults(
            const Session& session,
            OFIQ::FaceImageQualityPreprocessingResult& preprocessingResult,
            uint32_t resultRequestsMask) const;
    };
}

#endif /* OFIQ_LIB_IMPL_H */
