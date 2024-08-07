/**
 * @file adnet_landmarks.h
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
 * @brief Provides the \link OFIQ_LIB::modules::landmarks::ADNetFaceLandmarkExtractor 
 * ADNetFaceLandmarkExtractor\endlink class.
 * @author OFIQ development team
 */
#pragma once

#include <memory>
#include "Configuration.h"
#include "detectors.h"
#include "landmarks.h"

/**
 * @brief Provides implementations of a landmark extractors.
 */
namespace OFIQ_LIB::modules::landmarks
{
    
    /// Forward class declaration
    class ADNetFaceLandmarkExtractorImpl;

    /**
     * @brief Class implementing the \link OFIQ_LIB::FaceLandmarkExtractorInterface
     * FaceLandmarkExtractorInterface\endlink interface.
     * 
     */
    class ADNetFaceLandmarkExtractor : public FaceLandmarkExtractorInterface
    {
    public:
        /**
         * @brief Constructor
         * @param config Configuration object
         */
        explicit ADNetFaceLandmarkExtractor(const Configuration& config);

        /**
         * @brief Destructor 
         */
        ~ADNetFaceLandmarkExtractor() override;

    protected:
        /**
         * @brief Computes landmarks of the face detected in the session.
         * @details The landmarks are computed using ADNet.
         * @param session Session object containing preprocessing results
         * used by the function to compute the landmarks.
         * @return Facial landmarks.
         */
        OFIQ::FaceLandmarks updateLandmarks(OFIQ_LIB::Session& session) override;

    private:
        
        /**
         * @brief Encapsulated implementation class. 
         */
        std::unique_ptr<ADNetFaceLandmarkExtractorImpl> landmarkExtractor_;
    };
}
