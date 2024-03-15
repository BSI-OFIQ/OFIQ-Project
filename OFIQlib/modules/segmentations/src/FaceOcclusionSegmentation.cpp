/**
 * @file FaceOcclusionSegmentation.cpp
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
 * @author OFIQ development team
 */

#include "FaceOcclusionSegmentation.h"
#include "OFIQError.h"
#include "utils.h"
#include <string>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace OFIQ_LIB::modules::segmentations
{


    FaceOcclusionSegmentation::FaceOcclusionSegmentation(const Configuration& config)
    {
        std::string modelPath = config.getDataDir() + "/" + config.GetString(modelConfigItem);

        try
        {
            std::ifstream instream(modelPath, std::ios::in | std::ios::binary);
            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnv.initialize(modelData, scaledWidth, scaledHeight);
        }
        catch (const std::exception& e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::FaceOcclusionSegmentationError,
                std::string("Loading model for FaceOcclusionSegmentation failed: " + 
                    modelPath));
        }
    }

    cv::Mat FaceOcclusionSegmentation::GetFaceOcclusionSegmentation(const cv::Mat& alignedImage)
    {
        cv::Mat alignedCrop = alignedImage(
            cv::Range(cropTop, alignedImage.rows - cropBottom),
            cv::Range(cropLeft, alignedImage.cols - cropRight));
        int croppedWidth = alignedCrop.cols;
        int croppedHeight = alignedCrop.rows;
        cv::Size size(scaledWidth, scaledHeight);
        cv::Mat resized;
        cv::resize(alignedCrop, resized, size);
        float scaleFactor = 1/255.0f;
        cv::Mat blob = cv::dnn::blobFromImage({resized}, scaleFactor, cv::Size(), 0, true);

        // Convert cv::Mat to std::vector<float>
        std::vector<float> net_input;
        net_input.assign(blob.begin<float>(), blob.end<float>());

        size_t nbOutputNodes = m_onnxRuntimeEnv.getNumberOfOutputNodes();
        auto results = m_onnxRuntimeEnv.run(net_input);

        size_t useThisOutput = nbOutputNodes - 1;

        auto element = results[useThisOutput].GetTensorTypeAndShapeInfo();
        std::vector<int64_t> shape = element.GetShape();
        auto elementPtr = results[useThisOutput].GetTensorMutableData<float>();

        // Assuming 'tensorDims' contains dimensions like {batchSize, channels, height, width}
        auto batchSize = static_cast<int>(shape[0]);
        auto nbChannels = static_cast<int>(shape[1]);
        auto height = static_cast<int>(shape[2]);
        auto width = static_cast<int>(shape[3]);

        // Create a cv::Mat from the tensor data
        int sizeMat[] = {batchSize, nbChannels, height, width};
        auto mat = cv::Mat(4, sizeMat, CV_32FC1, elementPtr);

        cv::Mat outputReshaped(size, CV_32F, elementPtr);

        outputReshaped *= -1;
        cv::threshold(outputReshaped, outputReshaped, 0, 1, cv::THRESH_BINARY_INV);
        cv::Mat maskRescaled;
        cv::resize(
            outputReshaped,
            maskRescaled,
            cv::Size(croppedWidth, croppedHeight),
            0,
            0,
            cv::INTER_NEAREST);
        cv::Mat maskAligned = cv::Mat::zeros(alignedImage.size(), CV_64F);
        maskRescaled.copyTo(maskAligned(
            cv::Range(cropTop, croppedHeight + cropTop),
            cv::Range(cropLeft, croppedWidth + cropLeft)));
        maskAligned.convertTo(maskAligned, CV_8U);

        return maskAligned;
    }

    OFIQ::Image FaceOcclusionSegmentation::UpdateMask(
        OFIQ_LIB::Session& session, SegmentClassLabels faceSegment)
    {
        if (segmentationImage == nullptr || session.Id() != GetLastSessionId())
            try
            {
                segmentationImage =
                    std::make_shared<cv::Mat>(GetFaceOcclusionSegmentation(session.getAlignedFace()));
            }
            catch (const std::exception& e)
            {
                throw OFIQError(
                    OFIQ::ReturnCode::FaceOcclusionSegmentationError,
                    "Occlusion segment generation failed: " + std::string(e.what()));
            }

        OFIQ::Image maskImage =
            OFIQ_LIB::MakeGreyImage(segmentationImage->cols, segmentationImage->rows);


        if (OFIQ_LIB::modules::segmentations::SegmentClassLabels::face == faceSegment)
        {
            memcpy(maskImage.data.get(), segmentationImage->data, maskImage.size());
        }
        else
        {
            // nothing, this segmentation algorithm has only one layer
        }

        return maskImage;
    }

}