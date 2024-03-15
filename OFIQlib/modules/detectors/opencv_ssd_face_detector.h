/**
 * @file opencv_ssd_face_detector.h
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
 * @brief Implementation of a face detector using the SSD face detector CNN.
 * @author OFIQ development team
 */
#pragma once

#include "Configuration.h"
#include "detectors.h"
#include <opencv2/dnn.hpp>


/**
 * @brief Provides face detector implementations.
 */
namespace OFIQ_LIB::modules::detectors
{

    /**
     * @brief Implementation of a face detector using the SSD face detector CNN.
     */
    class SSDFaceDetector : public OFIQ_LIB::FaceDetectorInterface
    {
    public:
        /**
         * @brief Constructor a new SSDFaceDetector 
         * 
         * @param config 
         */
        explicit SSDFaceDetector(const Configuration& config);

        /**
         * @brief Destructor of the SSDFaceDetector 
         * 
         */
        ~SSDFaceDetector() override = default;

    protected:
        /**
         * @brief Implementation of the face detection method.
         * 
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method.
         * @return std::vector<OFIQ::BoundingBox> Bounding boxes of the detected faces
         */
        std::vector<OFIQ::BoundingBox> UpdateFaces(OFIQ_LIB::Session& session) override;


    private:
        /**
         * @brief Instance of an opencv dnn::Net.
         * 
         */
        std::shared_ptr<cv::dnn::Net> dnnNet{nullptr};

        /**
         * @brief Confidence threshold used for the face detection. The value is read from the configuration file.
         * 
         */
        double confidenceThreshold;
        
        /**
         * @brief Add padding around the image (faceImage.width * padding; faceImage.height * padding;)
         * 
         */
        double padding;
        
        /**
         * @brief Filter threshold for removing to small face found on the image. This value is read from the configuration file.
         * 
         */
        double minimalRelativeFaceSize;
    };
}
