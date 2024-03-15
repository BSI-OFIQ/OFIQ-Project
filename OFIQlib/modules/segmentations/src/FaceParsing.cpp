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
#include "OFIQError.h"
#include "utils.h"
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace OFIQ_LIB::modules::segmentations
{

    FaceParsing::FaceParsing(const Configuration& config)
    {
        std::string modelPath = config.getDataDir() + "/" + config.GetString(modelConfigItem);
        
        try
        {
            std::ifstream instream(modelPath, std::ios::in | std::ios::binary);

            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnv.initialize(modelData, imageSize, imageSize);
        }
        catch (const std::exception& e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::FaceParsingError,
                std::string("Loading model for FaceParsing failed: " + modelPath +
                ". Config dir: " + config.getDataDir() + 
                    ". Original exception: " + std::string(e.what())));
        }
    }

    void FaceParsing::SetImage(OFIQ_LIB::Session& session)
    {
        cv::Mat inputImage = session.getAlignedFace();
        cv::Mat croppedImage = inputImage(cv::Range(0, inputImage.rows - cropBottom), cv::Range(cropLeft, inputImage.cols - cropRight));
        cv::cvtColor(croppedImage, croppedImage, cv::COLOR_BGR2RGB);
        auto blob = FaceParsing::CreateBlob(croppedImage, imageSize);

        // Convert cv::Mat to std::vector<float>
        std::vector<float> net_input;
        net_input.assign(blob.begin<float>(), blob.end<float>());

        size_t nbOutputNodes = m_onnxRuntimeEnv.getNumberOfOutputNodes();
        auto results = m_onnxRuntimeEnv.run(net_input);
        
        size_t useThisOutput = 0;// nbOutputNodes - 1;

        auto element = results[useThisOutput].GetTensorTypeAndShapeInfo();
        std::vector<int64_t> shape = element.GetShape();
        auto elementPtr = results[useThisOutput].GetTensorMutableData<float>();
    
        // Assuming 'tensorDims' contains dimensions like {batchSize, channels, height, width}
        auto batchSize = static_cast<int>(shape[0]);
        auto nbChannels = static_cast<int>(shape[1]);
        auto height = static_cast<int>(shape[2]);
        auto width = static_cast<int>(shape[3]);

        // Create a cv::Mat from the tensor data
        int size[] = { batchSize, nbChannels, height, width};
        auto mat = cv::Mat(4, size, CV_32FC1, elementPtr);
        
        std::vector<cv::Mat> out;
        cv::dnn::imagesFromBlob(mat, out);

        segmentationImage = FaceParsing::CalculateClassIds(
            out[0],
            imageSize);

    }


    OFIQ::Image
        FaceParsing::UpdateMask(OFIQ_LIB::Session& session, SegmentClassLabels faceSegment)
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
        OFIQ::Image maskImage = OFIQ_LIB::MakeGreyImage(segmentationImage->cols, segmentationImage->rows);


        if (OFIQ_LIB::modules::segmentations::SegmentClassLabels::face == faceSegment) {
            memcpy(maskImage.data.get(), segmentationImage->data, maskImage.size());
        }
        else {
            if (auto channel = static_cast<uchar>(faceSegment); channel != 0)
            {
                cv::threshold(*segmentationImage, mask, channel, 255, cv::THRESH_TOZERO_INV);
                cv::threshold(mask, mask, channel - 1, 255, cv::THRESH_BINARY);
            }
            else
                cv::threshold(*segmentationImage, mask, channel, 255, cv::THRESH_BINARY_INV);

            auto kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, {3, 3});
            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
            cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

            memcpy(maskImage.data.get(), mask.data, maskImage.size());
        }

        return maskImage;
    }

    cv::Mat FaceParsing::CreateBlob(const cv::Mat& image, int imageSize)
    {     
        cv::Scalar mean(0.485, 0.456, 0.406);
        cv::Scalar std(0.229, 0.224, 0.225);

        cv::Size size(imageSize, imageSize);

        mean *= 255;
        float scaleFactor = 1 / 255.0f;
        cv::Mat blob = cv::dnn::blobFromImage({ image }, scaleFactor, size, mean);
        std::vector<cv::Mat> images;
        cv::dnn::imagesFromBlob(blob, images);
        cv::Mat out = images[0];
        out /= std;
        blob = cv::dnn::blobFromImage({ out });

        return blob;
    }

    std::shared_ptr<cv::Mat> FaceParsing::CalculateClassIds(
        const cv::Mat& resultImage, int imageSize_one_dim)
    {

        cv::Mat1f maxValues(cv::Size(imageSize_one_dim, imageSize_one_dim), -5000.0);
        auto output = cv::Mat1b(cv::Size(imageSize_one_dim, imageSize_one_dim), 25);
        std::vector<cv::Mat> channels;

        cv::split(resultImage, channels);

        #define FOREACHPIXEL for (int i = 0; i < imageSize_one_dim; i++) for (int j = 0; j < imageSize_one_dim; j++)

        for (uchar channelId = 0; channelId < channels.size(); channelId++)
        {
            auto& channel = channels[channelId];
            FOREACHPIXEL
            {
                auto value = channel.at<float>(cv::Point(i, j));
                auto& maxValue = maxValues.at<float>(cv::Point(i, j));
                if (value > maxValue)
                {
                    maxValue = value;
                    output.at<uchar>(cv::Point(i, j)) = channelId;
                }
            }
        }

        return std::make_shared<cv::Mat>(output);
    }

}