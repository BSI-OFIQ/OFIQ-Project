/**
 * @file HeadPose3DDFAV2.h
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
 * @brief Provides a class implementing a head pose estimator based on https://github.com/cleardusk/3DDFA_V2.
 * @author OFIQ development team
 */
#pragma once

#include "Configuration.h"
#include "poseEstimators.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/core/mat.hpp>

/**
 * @brief Provides implementation of a head pose estimator.
 * 
 */
namespace OFIQ_LIB::modules::poseEstimators
{
    /**
     * @brief Implementation of a head pose estimator.
     * @details The estimator is is based on a CNN from https://github.com/cleardusk/3DDFA_V2.
     */
    class HeadPose3DDFAV2 : public PoseEstimatorInterface
    {
    public:
        /**
         * @brief Constructor for HeadPose3DDFAV2.
         * 
         * @param config Configuration from where the the path
		 * to the CNN model in ONNX format to read.
         */
        explicit HeadPose3DDFAV2(const Configuration& config);

        /**
         * @brief Destructor
         */
        ~HeadPose3DDFAV2() override = default;

    protected:
        /**
         * @brief Computation of the head pose.
         * 
         * @param session Session object containing the original facial image and pre-processing results 
         * computed.
         * @param pose Estimated head pose.
         */
        void updatePose(OFIQ_LIB::Session& session, EulerAngle& pose) override;

    private:
        /**
         * @brief Name of the used CNN net, passed from the configuration.
         */
        static const std::string paramPoseEstimatorModel;

        /**
         * @brief ONNXRuntime environment handle.
         */
        Ort::Env m_ortenv;

        /**
         * @brief ONNXRuntime session handle.
         */
        std::unique_ptr<Ort::Session> m_ort_session;

        /**
         * @brief Width of the CNN used for computation, read from the loaded model.
         */
        int64_t m_expected_image_width = 0;

        /**
         * @brief Height of the CNN used for computation, read from the loaded model.
         */
        int64_t m_expected_image_height = 0;

        /**
         * @brief Expected number of channels of the input image, read from the loaded model.
         */
        int64_t m_expected_image_number_of_channels = 0;

        /**
         * @brief Number of input elements of the CNN used for computation, read from the loaded model.
         */
        int64_t m_number_of_input_elements = 0;

        /**
         * @brief inputShape of the CNN used for computation, read from the loaded model.
         */
        std::array<int64_t, 4> inputShape;

        /**
         * @brief Crop face from image. Internally the passed bounding box will be transformed to a square region.
         * 
         * @param image Input image.
         * @param biggestFace Input region to be cropped.
         * @return cv::Mat Cropped face region.
         */
        cv::Mat CropImage(const cv::Mat& image, const OFIQ::BoundingBox& biggestFace);
    };
}