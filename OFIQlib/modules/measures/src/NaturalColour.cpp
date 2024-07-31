/**
 * @file NaturalColour.cpp
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

#include "NaturalColour.h"
#include "FaceMeasures.h"
#include "FaceParts.h"
#include "image_utils.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace OFIQ_LIB::modules::measures
{
    using FaceMeasures = landmarks::FaceMeasures;
    using PartExtractor = landmarks::PartExtractor;
    using FaceParts = landmarks::FaceParts;

    static const auto qualityMeasure = OFIQ::QualityMeasure::NaturalColour;

    static bool IsColoured(const cv::Mat& image)
    {
        int channelCount = image.channels();

        if (channelCount != 3)
        {
            return false;
        }

        for (int i = 0; i < image.rows; i++)
        {
            for (int j = 0; j < image.cols; j++)
            {
                uchar c0 = image.data[channelCount*(i*image.cols+j)+0];
                uchar c1 = image.data[channelCount*(i*image.cols+j)+1];
                uchar c2 = image.data[channelCount*(i*image.cols+j)+2];
                if (c0 != c1 || c0 != c2 || c1 != c2)
                {
                    return true;
                }
            }
        }

        return false;
    }

    NaturalColour::NaturalColour(
        const Configuration& configuration)
        : Measure{ configuration, qualityMeasure }
    {
        SigmoidParameters defaultValues;
        defaultValues.h = 200.0;
        defaultValues.a = 1.0;
        defaultValues.s = -1.0;
        defaultValues.x0 = 0.0;
        defaultValues.w = 10.0;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void NaturalColour::Execute(OFIQ_LIB::Session & session)
    {
        auto landmarks = session.getAlignedFaceLandmarks();
        auto alignedFace = session.getAlignedFace();

        if (!IsColoured(alignedFace))
        {
            double D = 0.0;
            SetQualityMeasure(session, qualityMeasure, D, OFIQ::QualityMeasureReturnCode::Success);
            return;
        }

        cv::Mat cvMask = session.getAlignedFaceLandmarkedRegion();
        cv::Mat faceSegmentation;
        cv::bitwise_and(alignedFace, alignedFace, faceSegmentation, cvMask);

        cv::Mat maskedImage = CreateMaskedImage(landmarks, faceSegmentation);
        OFIQ::LandmarkPoint leftEyeCenter;
        OFIQ::LandmarkPoint rightEyeCenter;
        double interEyeDistance;
        double eyeMouthDistance;
        CalculateReferencePoints(landmarks, leftEyeCenter, rightEyeCenter, interEyeDistance, eyeMouthDistance);
        cv::Rect leftRegionOfInterest;
        cv::Rect rightRegionOfInterest;
        CalculateRegionOfInterest(leftRegionOfInterest, rightRegionOfInterest, leftEyeCenter, rightEyeCenter, interEyeDistance, eyeMouthDistance);
        cv::Mat reducedImage = ReduceImageToRegionOfInterest(maskedImage, leftRegionOfInterest, rightRegionOfInterest);
        double meanChannelA;
        double meanChannelB;
        ConvertBGRToCIELAB(reducedImage, meanChannelA, meanChannelB);
        double rawScore = CalculateScore(meanChannelA, meanChannelB);
        SetQualityMeasure(session, qualityMeasure, rawScore, OFIQ::QualityMeasureReturnCode::Success);
    }

    cv::Mat NaturalColour::CreateMaskedImage(const OFIQ::FaceLandmarks& landmarks, const cv::Mat& cvImage) const
    {
        auto cvMask = FaceMeasures::GetFaceMask(landmarks, cvImage.rows, cvImage.cols);
        cv::Mat maskedImage;
        cvImage.copyTo(maskedImage, cvMask);
        return maskedImage;
    }

    cv::Mat NaturalColour::ReduceImageToRegionOfInterest(
        const cv::Mat& maskedImage,
        const cv::Rect& leftRegionOfInterest,
        const cv::Rect& rightRegionOfInterest) const
    {
        auto leftRegion = maskedImage(leftRegionOfInterest);
        auto rightRegion = maskedImage(rightRegionOfInterest);
        cv::Mat reducedImage;
        cv::hconcat(std::vector{ rightRegion, leftRegion}, reducedImage);
        return reducedImage;
    }

    double NaturalColour::CalculateScore(double meanChannelA, double meanChannelB) const
    {
        auto rawScore = (meanChannelA >= 0 && meanChannelB >= 0)
                          ? sqrt(
                              std::pow(std::max(std::max(0.0, 5 - meanChannelA), std::max(0.0, meanChannelA - 25)), 2)
                            + std::pow(std::max(std::max(0.0, 5 - meanChannelB), std::max(0.0, meanChannelB - 35)), 2))
                          : 100;
        return rawScore;
    }
}