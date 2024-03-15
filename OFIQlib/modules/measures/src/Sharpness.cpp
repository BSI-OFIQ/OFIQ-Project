/**
 * @file Sharpness.cpp
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

#include "Sharpness.h"
#include "OFIQError.h"
#include <opencv2/ml.hpp>
#include "FaceMeasures.h"
#include "utils.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::Sharpness;
    static const std::string prefixPath = "params.measures.Sharpness.";
    static const std::string paramModelFile = prefixPath + "model_path";
    static const std::string faceRegionConfigItem = prefixPath + "face_region_alpha";
    static const std::string useAlignedConfigItem = prefixPath + "use_aligned_landmarks";

    Sharpness::Sharpness(
        const Configuration& configuration,
        Session& session)
    : Measure{ configuration, session, qualityMeasure }
    {
        if (!configuration.GetBool(useAlignedConfigItem, this->useAligned))
            this->useAligned = false;
        if (!configuration.GetNumber(faceRegionConfigItem, this->faceRegionAlpha))
            this->faceRegionAlpha = 0;

        if (!configuration.GetString(paramModelFile, this->modelFile) ||
            this->modelFile.empty())
            throw OFIQError(OFIQ::ReturnCode::UnknownError,
                std::string("Missing or empty value of parameter 'model_path' in Sharpness"));
        else
        {
            try
            {
                rtree = cv::ml::RTrees::load(configuration.getDataDir() + "/" + this->modelFile);
            }
            catch (const std::exception&)
            {
                throw OFIQError(OFIQ::ReturnCode::UnknownError,
                    std::string("Unable to initialize Sharpness from given file: ") + this->modelFile);
            }
        }

        this->numTrees = rtree->getTermCriteria().maxCount;

        SigmoidParameters defaultValues;
        defaultValues.h = 100;
        defaultValues.x0 = 20.0;
        defaultValues.w = 10.0;
        defaultValues.round = true;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void Sharpness::Execute(OFIQ_LIB::Session & session)
    {
        cv::Mat faceCrop, maskCrop;
        GetCroppedImages(session, faceCrop, maskCrop, useAligned, faceRegionAlpha);

        cv::Mat features = GetClassifierFocusFeatures(faceCrop, maskCrop, true);
        features.convertTo(features, CV_32F);

        cv::Mat predResults;
        float svmPrediction = this->rtree->predict(features, predResults, cv::ml::StatModel::RAW_OUTPUT);

        double prediction = numTrees - predResults.at<float>(0, 0);
        SetQualityMeasure(session, qualityMeasure, prediction, OFIQ::QualityMeasureReturnCode::Success);
    }

    void Sharpness::GetCroppedImages(Session& session, cv::Mat& faceCrop, cv::Mat& maskCrop, bool useAligned, float faceRegionAlpha)
    {
        cv::Mat img, faceMask;
        if (useAligned)
        {
            img = session.getAlignedFace();
            faceMask = session.getAlignedFaceLandmarkedRegion() * 255;
        }
        else
        {
            img = copyToCvImage(session.image());
            auto faceLandmarks = session.getLandmarks();
            faceMask = landmarks::FaceMeasures::GetFaceMask(faceLandmarks, img.rows, img.cols, faceRegionAlpha) * 255;
        }
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(faceMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
        cv::Rect rect = cv::boundingRect(contours[0]);
        faceCrop = img(rect);
        maskCrop = faceMask(rect);
    }

    cv::Mat Sharpness::GetClassifierFocusFeatures(cv::Mat& image, cv::Mat& mask, bool applyBlur)
    {
        cv::Mat features;
        cv::Mat grayImage = image.clone();
        if (image.channels() == 3)
        {
            cv::cvtColor(grayImage, grayImage, cv::COLOR_BGR2GRAY);
        }
        cv::Mat grayBlur3 = grayImage.clone();
        if (applyBlur)
        {
            cv::GaussianBlur(grayBlur3, grayBlur3, cv::Size(3, 3), 0);
        }
        // calculate Laplacian features
        int kernelSizes[5] = { 1, 3, 5, 7, 9 };
        for (int& k : kernelSizes)
        {
            cv::Mat laplacian, mean, stddev;
            cv::Laplacian(grayBlur3, laplacian, CV_64F, k);
            cv::Mat abs = cv::abs(laplacian);
            cv::meanStdDev(abs, mean, stddev, mask);
            features.push_back(mean.reshape(1));
            features.push_back(stddev.reshape(1));
        }
        // calculate Mean Diff features
        int kernelSizesMeanDiff[3] = { 3, 5, 7 };
        for (int& k : kernelSizesMeanDiff)
        {
            cv::Mat grayMeanBlur, absdiff, mean, stddev;
            cv::blur(grayImage, grayMeanBlur, cv::Size(k, k));
            cv::absdiff(grayImage, grayMeanBlur, absdiff);
            cv::meanStdDev(absdiff, mean, stddev, mask);
            features.push_back(mean.reshape(1));
            features.push_back(stddev.reshape(1));
        }
        // calculate Sobel features
        for (int& k : kernelSizes)
        {
            cv::Mat sobel, mean, stddev;
            cv::Sobel(grayImage, sobel, CV_64F, 1, 1, k);
            cv::Mat abs = cv::abs(sobel);
            cv::meanStdDev(abs, mean, stddev, mask);
            features.push_back(mean.reshape(1));
            features.push_back(stddev.reshape(1));
        }
        cv::transpose(features, features);
        return features;
    }
}