/**
 * @file NeuralNetworkContainer.h
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
 * @brief Provides a neural network container structure.
 * @author OFIQ development team
 */
#pragma once
#include "detectors.h"
#include "landmarks.h"
#include "segmentations.h"
#include "poseEstimators.h"
 
/**
 * @brief Namespace for OFIQ implementations.
 */
namespace OFIQ_LIB
{
    /**
     * @brief Neural network container for OFIQ's preprocessing steps. 
     */
    struct NeuronalNetworkContainer
    {
        /**
         * @brief Constructor
         * @param faceDetector Implementation of a \link OFIQ_LIB::FaceDetectorInterface FaceDetectorInterface \endlink
         * @param landmarkExtractor Implementation of a \link OFIQ_LIB::FaceLandmarkExtractorInterface FaceLandmarkExtractorInterface \endlink
         * @param segmentationExtractor Implementation of a \link OFIQ_LIB::SegmentationExtractorInterface SegmentationExtractorInterface \endlink.
         * A pointer to an object instantiated from the \link OFIQ_LIB::modules::segmentations::FaceParsing FaceParsing\endlink class would
         * be valid.
         * @param poseEstimator Implementation of a \link OFIQ_LIB::PoseEstimatorInterface PoseEstimatorInterface \endlink
         * @param faceOcclusionExtractor Implementation of a \link OFIQ_LIB::SegmentationExtractorInterface SegmentationExtractorInterface \endlink.
         * A pointer to an object instantiated from the \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation FaceOcclusionSegmentation\endlink class would
         * be valid.
         */
        NeuronalNetworkContainer(
            std::shared_ptr<FaceDetectorInterface> faceDetector,
            std::shared_ptr<FaceLandmarkExtractorInterface> landmarkExtractor,
            std::shared_ptr<SegmentationExtractorInterface> segmentationExtractor,
            std::shared_ptr<PoseEstimatorInterface> poseEstimator,
            std::shared_ptr<SegmentationExtractorInterface> faceOcclusionExtractor
            )
            : faceDetector{faceDetector},
              landmarkExtractor{landmarkExtractor},
              segmentationExtractor{segmentationExtractor},
              poseEstimator{poseEstimator},
              faceOcclusionExtractor{faceOcclusionExtractor}
        {
        }

        /**
         * @brief Pointer to a \link OFIQ_LIB::FaceDetectorInterface FaceDetectorInterface \endlink
         */
        std::shared_ptr<FaceDetectorInterface> faceDetector;

        /**
         * @brief Pointer to a \link OFIQ_LIB::FaceLandmarkExtractorInterface FaceLandmarkExtractorInterface \endlink
         */
        std::shared_ptr<FaceLandmarkExtractorInterface> landmarkExtractor;

        /**
         * @brief Pointer to a \link OFIQ_LIB::SegmentationExtractorInterface SegmentationExtractorInterface \endlink
         * @details A pointer to an object instantiated from the \link OFIQ_LIB::modules::segmentations::FaceParsing FaceParsing\endlink class would
         * be valid.
         */
        std::shared_ptr<SegmentationExtractorInterface> segmentationExtractor;

        /**
         * @brief Pointer to a \link OFIQ_LIB::SegmentationExtractorInterface SegmentationExtractorInterface \endlink
         * @details A pointer to an object instantiated from the \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation FaceOcclusionSegmentation\endlink class would
         * be valid.
         */
        std::shared_ptr<SegmentationExtractorInterface> faceOcclusionExtractor;

        /**
         * @brief Pointer to a \link OFIQ_LIB::SegmentationExtractorInterface SegmentationExtractorInterface \endlink 
         */
        std::shared_ptr<PoseEstimatorInterface> poseEstimator;
    };
}