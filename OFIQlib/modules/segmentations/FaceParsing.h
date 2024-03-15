/**
 * @file FaceParsing.h
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
 * @brief Provides a class implementing the face parsing
 * pre-processing.
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
     * @details Implements a [BiSeNet](https://github.com/zllrunning/face-parsing.PyTorch)-based
     * face parsing. The aligned face image is cropped and then scaled to the dimension 400 x 400.
     * All pixels of the resulting image are assigned to one of the following class.
     * <table>
     *  <tr><td>value</td><td>class</td></tr>
     *  <tr><td>0</td><td>background</td></tr>
     *  <tr><td>1</td><td>face skin</td></tr>
     *  <tr><td>2</td><td>left eye brow</td></tr>
     *  <tr><td>3</td><td>right eye brow</td></tr>
     *  <tr><td>4</td><td>left eye</td></tr>
     *  <tr><td>5</td><td>right eye</td></tr>
     *  <tr><td>6</td><td>eyeglasses</td></tr>
     *  <tr><td>7</td><td>left ear</td></tr>
     *  <tr><td>8</td><td>right ear</td></tr>
     *  <tr><td>9</td><td>earring</td></tr>
     *  <tr><td>10</td><td>nose</td></tr>
     *  <tr><td>11</td><td>mouth</td></tr>
     *  <tr><td>12</td><td>upper lip</td></tr>
     *  <tr><td>13</td><td>lower lip</td></tr>
     *  <tr><td>14</td><td>neck</td></tr>
     *  <tr><td>15</td><td>necklace</td></tr>
     *  <tr><td>16</td><td>clothing</td></tr>
     *  <tr><td>17</td><td>hair</td></tr>
     *  <tr><td>18</td><td>head covering</td></tr>
     * </table>
     * The result of face parsing is an image (matrix) of dimension
     * 400 x 400 where each pixel is assigned with one of the values
     * listed in the table from above.
     */
    class FaceParsing : public SegmentationExtractorInterface
    {
    public:
        /**
         * @brief Constructor
         * @param config Configuration object from which related
         * parameters may be read.
         * @see For configuration of face parsing, see @ref sec_required_cfg
         */
        explicit FaceParsing(const Configuration& config);

        /**
         * @brief Destructor
         */
        ~FaceParsing() override = default;


    protected:
        /**
         * @brief Implements face parsing.
         *
         * @details The function is invoked by \link OFIQ_LIB::SegmentationExtractorInterface::GetMask()
         * SegmentationExtractorInterface::GetMask()\endlink. It crops the aligned face image returned
         * by \link OFIQ_LIB::Session::getAlignedFace() Session::getAlignedFace()\endlink as configured
         * by private member variables. The result is scaled to the dimension of 400 x 400 and
         * passed to the [BiSeNet](https://github.com/zllrunning/face-parsing.PyTorch) CNN. The output
         * is returned as face parsing.
         *
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         *
         * @param faceSegment Enum value encoding the requested face segment. If the value is
         * \link OFIQ_LIB::modules::segmentations::SegmentClassLabels::face SegmentClassLabels::face\endlink
         * then an image of dimension 400 x 400 is returned assigning each pixel a value between 0 and 18 as
         * in the table of \link OFIQ_LIB::modules::segmentations::FaceParsing FaceParsing\endlink; otherwise
         * a binary mask of dimension 400 x 400 is returned with the requested face segment and 
         * morphologically extended (kernel size 3) is returned.
         *
         * @return Face parsing image of dimension 400 x 400 where each pixel is assigned with one of the values
         * described in the table of the \link OFIQ_LIB::modules::segmentations::FaceParsing \endlink 
         * class documentation.
         */
        OFIQ::Image UpdateMask(
            OFIQ_LIB::Session& session, modules::segmentations::SegmentClassLabels faceSegment) override;

    private:

        /**
         * @brief Manages CNN computations.
         */
        ONNXRuntimeSegmentation m_onnxRuntimeEnv;

        /**
         * @brief Stores the last result computed with
         * \link OFIQ_LIB::modules::segmentations::FaceParsing::UpdateMask()
         * UpdateMask()\endlink.
         */
        std::shared_ptr<cv::Mat> segmentationImage;

        /**
         * @brief JSON/JAXN key to access path to [BiSeNet](https://github.com/zllrunning/face-parsing.PyTorch)
         * model in ONNX format from
         * \link OFIQ_LIB::Configuration Configuration\endlink object.
         */
        const std::string modelConfigItem = "params.measures.FaceParsing.model_path";

        /**
         * @brief Face parsing target dimension. 
         */
        const int imageSize = 400;

        /**
         * @brief Cropping parameter. 
         */
        const int cropLeft = 30;

        /**
         * @brief Cropping parameter.
         */
        const int cropRight = 30;

        /**
         * @brief Cropping parameter.
         */
        const int cropTop = 0;

        /**
         * @brief Cropping parameter.
         */
        const int cropBottom = 60;
        
        /**
         * @brief Creates the blob being input to the face parsing CNN.
         * @param image Input image
         * @param i_imageSize_one_dim Specifies the size of the blob being
         * input to the face parsing CNN; should be 400, such that a blob
         * of dimension 400 x 400 is created.
         * @return Blob of requested dimension.
         */
        static cv::Mat CreateBlob(const cv::Mat& image, int i_imageSize_one_dim);

        /**
         * @brief Applies segmentation to the blob created from the input image
         * and returns the result.
         * @details Is invoked by \link OFIQ_LIB::modules::segmentations::FaceParsing::SetImage()
         * SetImage()\endlink.
         * @param resultImage Blob being created by one of the CreateBlob functions.
         * @param i_imageSize_one_dim Specifies the size of the blob being
         * input to the face parsing CNN; should be 400, such that a blob
         * of dimension 400 x 400 is created.
         * @return Result of face parsing.
         */
        static std::shared_ptr<cv::Mat> CalculateClassIds(
            const cv::Mat& resultImage,
            int i_imageSize_one_dim);

        /*/
         * @brief Derives the private member \link segmentationImage\endlink
         * from the facial image data provided by the session object.
         * @details Implements CNN processing step of \link OFIQ_LIB::modules::segmentations::FaceParsing::UpdateMask()
         * UpdateMask()\endlink.
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         */
        void SetImage(OFIQ_LIB::Session& session);
    };
}