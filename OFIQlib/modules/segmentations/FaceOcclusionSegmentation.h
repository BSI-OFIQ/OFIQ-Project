/**
 * @file FaceOcclusionSegmentation.h
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
 * @brief Provides a class for segmenting the facial part not occluded by
 * any non-facial parts from an image.
 * @author OFIQ development team
 */
#pragma once

#include "Configuration.h"
#include "segmentations.h"
#include <ONNXRTSegmentation.h>

/**
 * @brief OpenCV's namespace. 
 */
namespace cv
{
    /**
     * @brief Forward declaration of the OpenCV class cv::Mat. 
     */
    class Mat;
}

/**
 * @brief Provides segmentation-related implementations.
 */
namespace OFIQ_LIB::modules::segmentations
{
    /**
     * @brief Class managing the separation of facial parts not occluded by non-facial
     * parts from other parts.
     * @details The implementation is based on a CNN from 
     * [FaceExtraction](https://github.com/face3d0725/FaceExtraction).
     */
    class FaceOcclusionSegmentation : public SegmentationExtractorInterface
    {
    public:
        /**
         * @brief Constructor 
         * @param config Configuration object from which some segmentation-related
         * parameters may be read.
         * @see @ref sec_required_cfg
         */
        explicit FaceOcclusionSegmentation(const Configuration& config);

        /**
         * @brief Destructor 
         */
        ~FaceOcclusionSegmentation() override = default;


    protected:
        /**
         * @brief Implements face occlusion segmentation.
         *
         * @details The function is invoked by \link OFIQ_LIB::SegmentationExtractorInterface::GetMask()
         * SegmentationExtractorInterface::GetMask()\endlink. Invokes 
         * \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation::GetFaceOcclusionSegmentation()
         * GetFaceOcclusionSegmentation()\endlink and stores its output in the private
         * \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation::segmentationImage 
         * segmentationImage\endlink member.
         * 
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         * 
         * @param faceSegment Should be the value 
         * \link OFIQ_LIB::modules::segmentations::SegmentClassLabels::face SegmentClassLabels::face\endlink.
         *
         * @return Face occlusion segmentation mask.
         */
        OFIQ::Image UpdateMask(
            OFIQ_LIB::Session& session, modules::segmentations::SegmentClassLabels faceSegment) override;

    private:

        /**
         * @brief Does the actual CNN-based occlusion-aware segmentation.
         * @param alignedImage Aligned image of dimension 616 x 616 as returned by 
         * \link OFIQ_LIB::Session::getAlignedFace() Session::getAlignedFace()\endlink.
         * @return Image where a pixel belonging to non-occluded facial parts is 
         * encoded as the byte value 1 and pixels belonging to other parts are encoded by the byte value 0.
         */
        cv::Mat GetFaceOcclusionSegmentation(const cv::Mat& alignedImage);

        /**
         * @brief Manages CNN computations.
         */
        ONNXRuntimeSegmentation m_onnxRuntimeEnv;
        
        /**
         * @brief Stores the last result computed with 
         * \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation::UpdateMask()
         * UpdateMask()\endlink. 
         */
        std::shared_ptr<cv::Mat> segmentationImage;

        /**
         * @brief JSON/JAXN key to access path to FaceExtraction's model file from 
         * \link OFIQ_LIB::Configuration Configuration\endlink object. 
         */
        const std::string modelConfigItem = "params.measures.FaceOcclusionSegmentation.model_path";

        /**
         * @brief Cropping parameter 
         */
        const int cropLeft = 96;

        /**
         * @brief Cropping parameter
         */
        const int cropRight = 96;
        
        /**
         * @brief Cropping parameter
         */
        const int cropTop = 96;
        
        /**
         * @brief Cropping parameter
         */
        const int cropBottom = 96;

        /**
         * @brief After cropping the aligned image, the result will be 
         * scaled to a dimension of that width for being input to the 
         * CNN-based segmentation.
         */
        const int scaledWidth = 224;

        /**
         * @brief After cropping the aligned image, the result will be 
         * scaled to a dimension of that height for being input to the
         * CNN-based segmentation.
         */
        const int scaledHeight = 224;

    };
}
