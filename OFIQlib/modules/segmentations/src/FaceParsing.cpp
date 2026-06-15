/**
 * @file FaceParsing.cpp
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

#include "FaceParsing.h"
#include "DataStream.h"
#include "OFIQError.h"
#include "utils.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>

namespace OFIQ_LIB::modules::segmentations
{

    FaceParsing::FaceParsing(const Configuration& config)
    {
        auto modelPath = config.GetFullPath(m_modelConfigItem);

        try
        {
            DataStream instream(modelPath, std::ios::in | std::ios::binary);

            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnv.initialize(modelData, m_imageSize, m_imageSize, config);
        }
        catch (const std::exception& e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::FaceParsingError,
                std::string(
                    "Loading model for FaceParsing failed: " + modelPath + ". Config dir: " +
                    config.getDataDir() + ". Original exception: " + std::string(e.what())));
        }
    }

    void FaceParsing::SetImage(const OFIQ_LIB::Session& session)
    {
        const cv::Mat& inputImage = session.getAlignedFace();
        cv::Mat croppedImage = inputImage(
            cv::Range(0, inputImage.rows - m_cropBottom),
            cv::Range(m_cropLeft, inputImage.cols - m_cropRight));
        cv::Mat croppedRGB;
        cv::cvtColor(croppedImage, croppedRGB, cv::COLOR_BGR2RGB);

        const cv::Scalar mean255 = cv::Scalar(0.485, 0.456, 0.406) * 255.0;
        const cv::Scalar std255 = cv::Scalar(0.229, 0.224, 0.225) * 255.0;
        cv::Mat transformed;
        croppedRGB.convertTo(transformed, CV_32FC3);
        transformed -= mean255;
        transformed /= std255;
        cv::Mat blob3 =
            cv::dnn::blobFromImage({transformed}, 1.0, cv::Size(m_imageSize, m_imageSize));

        // Convert cv::Mat to std::vector<float>
        std::vector<float> net_input;
        net_input.assign(blob3.begin<float>(), blob3.end<float>());

        auto results = m_onnxRuntimeEnv.run(net_input);

        size_t useThisOutput = 0;

        auto element = results[useThisOutput].GetTensorTypeAndShapeInfo();
        std::vector<int64_t> shape = element.GetShape();
        auto elementPtr = results[useThisOutput].GetTensorMutableData<float>();

        // Assuming 'tensorDims' contains dimensions like {batchSize, channels, height, width}
        auto batchSize = static_cast<int>(shape[0]);
        auto nbChannels = static_cast<int>(shape[1]);
        auto height = static_cast<int>(shape[2]);
        auto width = static_cast<int>(shape[3]);

        // Create a cv::Mat from the tensor data
        std::array<int, 4> size = {batchSize, nbChannels, height, width};
        auto mat = cv::Mat(4, size.data(), CV_32FC1, elementPtr);

        std::vector<cv::Mat> out;
        cv::dnn::imagesFromBlob(mat, out);

        m_segmentationImage = FaceParsing::CalculateClassIds(out[0], m_imageSize);
    }


    OFIQ::Image FaceParsing::UpdateMask(OFIQ_LIB::Session& session, SegmentClassLabels faceSegment)
    {
        try
        {
            SetImage(session);
        }
        catch (const std::exception& e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::FaceParsingError,
                "Face parsing failed: " + std::string(e.what()));
        }

        cv::Mat mask;
        OFIQ::Image maskImage = OFIQ_LIB::MakeGreyImage(
            static_cast<uint16_t>(m_segmentationImage->cols),
            static_cast<uint16_t>(m_segmentationImage->rows));

        if (OFIQ_LIB::modules::segmentations::SegmentClassLabels::face == faceSegment)
        {
            memcpy(maskImage.data.get(), m_segmentationImage->data, maskImage.size());
        }
        else
        {
            if (auto channel = static_cast<uchar>(faceSegment); channel != 0)
            {
                cv::threshold(*m_segmentationImage, mask, channel, 255, cv::THRESH_TOZERO_INV);
                cv::threshold(mask, mask, channel - 1, 255, cv::THRESH_BINARY);
            }
            else
                cv::threshold(*m_segmentationImage, mask, channel, 255, cv::THRESH_BINARY_INV);

            auto kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, {3, 3});
            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
            cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

            memcpy(maskImage.data.get(), mask.data, maskImage.size());
        }

        return maskImage;
    }

    std::shared_ptr<cv::Mat>
        FaceParsing::CalculateClassIds(const cv::Mat& faceSegments, int imgSize)
    {
        auto classes = cv::Mat1b(cv::Size(imgSize, imgSize), 25);
        using Vec19f = cv::Vec<float, 19>;
        faceSegments.forEach<Vec19f>(
            [&classes](const Vec19f& pix, const int coords[])
            {
                // std::array<int, 2> maxIdx;
                int maxIdx[2];
                cv::minMaxIdx(pix, nullptr, nullptr, nullptr, maxIdx);
                classes.at<uint8_t>(coords[0], coords[1]) = (uint8_t)maxIdx[0];
            });
        return std::make_shared<cv::Mat>(classes);
    }
}