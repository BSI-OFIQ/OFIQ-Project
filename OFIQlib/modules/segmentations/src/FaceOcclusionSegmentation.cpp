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
#include "DataStream.h"
#include "OFIQError.h"
#include "utils.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

namespace OFIQ_LIB::modules::segmentations
{
    FaceOcclusionSegmentation::FaceOcclusionSegmentation(const Configuration& config)
    {
        auto modelPath = config.GetFullPath(m_modelConfigItem);

        try
        {
            DataStream instream(modelPath, std::ios::in | std::ios::binary);
            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnv.initialize(modelData, m_scaledWidth, m_scaledHeight, config);
        }
        catch (const std::exception&)
        {
            throw OFIQError(
                OFIQ::ReturnCode::FaceOcclusionSegmentationError,
                std::string("Loading model for FaceOcclusionSegmentation failed: " + modelPath));
        }
    }

    cv::Mat FaceOcclusionSegmentation::GetFaceOcclusionSegmentation(const cv::Mat& alignedImage)
    {
        const cv::Mat alignedCrop = alignedImage(
            cv::Range(m_cropTop, alignedImage.rows - m_cropBottom),
            cv::Range(m_cropLeft, alignedImage.cols - m_cropRight));
        int croppedWidth = alignedCrop.cols;
        int croppedHeight = alignedCrop.rows;
        cv::Size size(m_scaledWidth, m_scaledHeight);
        cv::Mat resized;
        cv::resize(alignedCrop, resized, size);
        float scaleFactor = 1 / 255.0f;
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

        cv::Mat outputReshaped(size, CV_32F, elementPtr);
        outputReshaped *= -1;
        cv::threshold(outputReshaped, outputReshaped, 0, 1, cv::THRESH_BINARY_INV);

        cv::Mat output8u = cv::Mat::zeros(size, CV_8U);
        outputReshaped.convertTo(output8u, CV_8U);
        cv::resize(
            output8u,
            output8u,
            cv::Size(croppedWidth, croppedHeight),
            0,
            0,
            cv::INTER_NEAREST);
        cv::Mat maskAligned8U = cv::Mat::zeros(alignedImage.size(), CV_8U);
        output8u.copyTo(maskAligned8U(
            cv::Range(m_cropTop, croppedHeight + m_cropTop),
            cv::Range(m_cropLeft, croppedWidth + m_cropLeft)));

        return maskAligned8U;
    }

    OFIQ::Image FaceOcclusionSegmentation::UpdateMask(
        OFIQ_LIB::Session& session, SegmentClassLabels faceSegment)
    {
        if (m_segmentationImage == nullptr || session.Id() != GetLastSessionId())
            try
            {
                m_segmentationImage = std::make_shared<cv::Mat>(
                    GetFaceOcclusionSegmentation(session.getAlignedFace()));
            }
            catch (const std::exception& e)
            {
                throw OFIQError(
                    OFIQ::ReturnCode::FaceOcclusionSegmentationError,
                    "Occlusion segment generation failed: " + std::string(e.what()));
            }

        OFIQ::Image maskImage = OFIQ_LIB::MakeGreyImage(
            static_cast<uint16_t>(m_segmentationImage->cols),
            static_cast<uint16_t>(m_segmentationImage->rows));

        if (OFIQ_LIB::modules::segmentations::SegmentClassLabels::face == faceSegment)
            memcpy(maskImage.data.get(), m_segmentationImage->data, maskImage.size());

        return maskImage;
    }

}