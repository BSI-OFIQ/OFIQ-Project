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
#include <array>

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
                uchar c0 = image.data[channelCount * (i * image.cols + j) + 0];
                uchar c1 = image.data[channelCount * (i * image.cols + j) + 1];
                uchar c2 = image.data[channelCount * (i * image.cols + j) + 2];
                if (c0 != c1 || c0 != c2 || c1 != c2)
                {
                    return true;
                }
            }
        }

        return false;
    }

    NaturalColour::NaturalColour(const Configuration& configuration)
        : Measure{configuration, qualityMeasure}
    {
        SigmoidParameters defaultValues;
        defaultValues.h = 200.0;
        defaultValues.a = 1.0;
        defaultValues.s = -1.0;
        defaultValues.x0 = 0.0;
        defaultValues.w = 10.0;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void NaturalColour::Execute(OFIQ_LIB::Session& session)
    {
        auto landmarks = session.getAlignedFaceLandmarks();
        const cv::Mat& alignedFace = session.getAlignedFace();

        if (!IsColoured(alignedFace))
        {
            SetQualityMeasure(session, qualityMeasure, 0.0, OFIQ::QualityMeasureReturnCode::Success);
            return;
        }

        const auto& cvMask = session.getAlignedFaceLandmarkedRegion();
        cv::Mat faceSegmentation;
        cv::bitwise_and(alignedFace, alignedFace, faceSegmentation, cvMask);
        cv::Mat maskedImage = CreateMaskedImage(landmarks, faceSegmentation);

        cv::Rect leftRegionOfInterest;
        cv::Rect rightRegionOfInterest;
        ComputeRegionsOfInterest(landmarks, leftRegionOfInterest, rightRegionOfInterest);

        cv::Mat reducedImage =
            ReduceImageToRegionOfInterest(maskedImage, leftRegionOfInterest, rightRegionOfInterest);
        if (reducedImage.empty())
        {
            SetQualityMeasure(
                session,
                qualityMeasure,
                100.0,
                OFIQ::QualityMeasureReturnCode::FailureToAssess);
            return;
        }
        double meanChannelA;
        double meanChannelB;
        ConvertBGRToCIELAB(reducedImage, meanChannelA, meanChannelB);
        double rawScore = CalculateScore(meanChannelA, meanChannelB);
        SetQualityMeasure(
            session,
            qualityMeasure,
            rawScore,
            OFIQ::QualityMeasureReturnCode::Success);
    }

    void NaturalColour::Visualize(OFIQ_LIB::Session& session, std::vector<uint32_t>& argbImage)
    {
        const auto landmarks = session.getAlignedFaceLandmarks();

        cv::Rect leftRegionOfInterest;
        cv::Rect rightRegionOfInterest;
        ComputeRegionsOfInterest(landmarks, leftRegionOfInterest, rightRegionOfInterest);

        cv::Mat alignedToOriginalTransform;
        cv::invertAffineTransform(
            session.getAlignedFaceTransformationMatrix(), alignedToOriginalTransform);
        const cv::Matx23d alignedToOriginal(alignedToOriginalTransform);

        const int width = session.image().width;
        const int height = session.image().height;
        cv::Mat mask = cv::Mat::zeros(height, width, CV_8UC1);
        cv::fillConvexPoly(
            mask, TransformRegionToImage(leftRegionOfInterest, alignedToOriginal), cv::Scalar(255));
        cv::fillConvexPoly(
            mask, TransformRegionToImage(rightRegionOfInterest, alignedToOriginal), cv::Scalar(255));

        BuildRegionOverlay(mask, argbImage);
    }

    cv::Mat NaturalColour::CreateMaskedImage(
        const OFIQ::FaceLandmarks& landmarks, const cv::Mat& cvImage) const
    {
        auto cvMask = FaceMeasures::GetFaceMask(landmarks, cvImage.rows, cvImage.cols);
        cv::Mat maskedImage;
        cvImage.copyTo(maskedImage, cvMask);
        return maskedImage;
    }

    void NaturalColour::ComputeRegionsOfInterest(
        const OFIQ::FaceLandmarks& landmarks,
        cv::Rect& leftRegionOfInterest,
        cv::Rect& rightRegionOfInterest) const
    {
        OFIQ::LandmarkPoint leftEyeCenter;
        OFIQ::LandmarkPoint rightEyeCenter;
        double interEyeDistance;
        double eyeMouthDistance;
        CalculateReferencePoints(
            landmarks, leftEyeCenter, rightEyeCenter, interEyeDistance, eyeMouthDistance);
        CalculateRegionOfInterest(
            leftRegionOfInterest,
            rightRegionOfInterest,
            leftEyeCenter,
            rightEyeCenter,
            interEyeDistance,
            eyeMouthDistance);
    }

    std::vector<cv::Point> NaturalColour::TransformRegionToImage(
        const cv::Rect& region, const cv::Matx23d& alignedToOriginal) const
    {
        const std::array<cv::Point2d, 4> corners = {
            cv::Point2d(region.x, region.y),
            cv::Point2d(region.x + region.width, region.y),
            cv::Point2d(region.x + region.width, region.y + region.height),
            cv::Point2d(region.x, region.y + region.height),
        };

        std::vector<cv::Point> polygon;
        polygon.reserve(corners.size());
        for (const auto& corner : corners)
        {
            const cv::Point2d mapped = alignedToOriginal * cv::Vec3d(corner.x, corner.y, 1.0);
            polygon.emplace_back(cvRound(mapped.x), cvRound(mapped.y));
        }
        return polygon;
    }

    void NaturalColour::BuildRegionOverlay(
        const cv::Mat& mask, std::vector<uint32_t>& argbImage) const
    {
        // Everything outside the regions of interest is dimmed with a
        // semi-transparent colour; the regions themselves stay fully transparent.
        constexpr uint32_t dimmed = (0x7Fu << 24)+(0xFF<<16)+(0xFF<<8)+(0xFF);
        constexpr uint32_t transparent = 0x00000000u;

        argbImage.assign(static_cast<size_t>(mask.rows) * mask.cols, dimmed);
        for (int y = 0; y < mask.rows; y++)
        {
            const uchar* maskRow = mask.ptr<uchar>(y);
            for (int x = 0; x < mask.cols; x++)
            {
                if (maskRow[x])
                {
                    argbImage[static_cast<size_t>(y) * mask.cols + x] = transparent;
                }
            }
        }
    }

    cv::Mat NaturalColour::ReduceImageToRegionOfInterest(
        const cv::Mat& maskedImage,
        const cv::Rect& leftRegionOfInterest,
        const cv::Rect& rightRegionOfInterest) const
    {
        auto leftRegion = maskedImage(leftRegionOfInterest);
        auto rightRegion = maskedImage(rightRegionOfInterest);
        cv::Mat reducedImage;
        cv::hconcat(std::vector{rightRegion, leftRegion}, reducedImage);
        return reducedImage;
    }

    double NaturalColour::CalculateScore(double meanChannelA, double meanChannelB) const
    {
        auto rawScore = (meanChannelA >= 0 && meanChannelB >= 0)
                          ? sqrt(
                                std::pow(
                                    std::max(
                                        std::max(0.0, 5 - meanChannelA),
                                        std::max(0.0, meanChannelA - 25)),
                                    2) +
                                std::pow(
                                    std::max(
                                        std::max(0.0, 5 - meanChannelB),
                                        std::max(0.0, meanChannelB - 35)),
                                    2))
                          : 100;
        return rawScore;
    }
}