/**
 * @file utils.cpp
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


#include "utils.h"
#include "FaceParts.h"
#include "OFIQError.h"
#include "PartExtractor.h"

#include <algorithm>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#define _USE_MATH_DEFINES
#include <math.h>


using PartExtractor = OFIQ_LIB::modules::landmarks::PartExtractor;
using FaceParts = OFIQ_LIB::modules::landmarks::FaceParts;

namespace OFIQ_LIB
{


    OFIQ_EXPORT void makeSquareBoundingBoxWithPadding(
        const OFIQ::BoundingBox& i_bb,
        const cv::Mat& i_input_image,
        cv::Mat& o_output_image,
        OFIQ::BoundingBox& o_bb,
        Point2i & o_translation_vector)
    {
        int height_image = i_input_image.rows;
        int width_image = i_input_image.cols;

        o_translation_vector.x = 0;
        o_translation_vector.y = 0;

        o_bb = OFIQ_LIB::makeSquareBoundingBox(i_bb);


        int x_top_left = o_bb.xleft;
        int y_top_left = o_bb.ytop;
        int x_bottom_right = x_top_left + o_bb.width;
        int y_bottom_right = y_top_left + o_bb.height;

        if ((x_top_left < 0) || (x_bottom_right >= width_image) || (y_top_left < 0) ||
            (y_bottom_right >= height_image))
        {
            // Define the border sizes
            int topBorder = std::max(0, -y_top_left);
            int bottomBorder = std::max(0, y_bottom_right - height_image + 1);
            int leftBorder = std::max(0, -x_top_left);
            int rightBorder = std::max(0, x_bottom_right - width_image + 1);


            o_translation_vector.x = leftBorder;
            o_translation_vector.y = topBorder;

            // adapt coordinates
            o_bb.xleft += o_translation_vector.x;
            o_bb.ytop += o_translation_vector.y;

            cv::copyMakeBorder(
                i_input_image,
                o_output_image,
                topBorder,
                bottomBorder,
                leftBorder,
                rightBorder,
                cv::BORDER_CONSTANT,
                cv::Scalar(0, 0, 0));
        }
        else
        {
            o_output_image = i_input_image.clone();
        }
    }

    OFIQ_EXPORT OFIQ::BoundingBox makeSquareBoundingBox(const OFIQ::BoundingBox& i_bb)
    {

        // if already square
        if (i_bb.height == i_bb.width)
        {
            return i_bb;
        }

        int16_t x_top_left_orig = i_bb.xleft;
        int16_t y_top_left_orig = i_bb.ytop;

        int16_t x_bottom_right_orig = i_bb.xleft + i_bb.width;
        int16_t y_bottom_right_orig = i_bb.ytop + i_bb.height;

        int16_t x_top_left_trans, y_top_left_trans, x_bottom_right_trans, y_bottom_right_trans = 0;

        // extend bounding box to square
        if (i_bb.width < i_bb.height)
        {
            // extend width

            x_top_left_trans =
                std::floor((x_bottom_right_orig + x_top_left_orig - i_bb.height) * 0.5);
            x_bottom_right_trans =
                std::ceil((x_bottom_right_orig + x_top_left_orig + i_bb.height) * 0.5);

            y_top_left_trans = y_top_left_orig;
            y_bottom_right_trans = y_bottom_right_orig;
        }
        else
        {
            // extend height
            x_top_left_trans = x_top_left_orig;
            x_bottom_right_trans = x_bottom_right_orig;

            y_top_left_trans =
                std::floor((y_bottom_right_orig + y_top_left_orig - i_bb.width) * 0.5);
            y_bottom_right_trans =
                std::ceil((y_bottom_right_orig + y_top_left_orig + i_bb.width) * 0.5);
        }

        OFIQ::BoundingBox square(
            x_top_left_trans // int16_t xleft
            ,
            y_top_left_trans // int16_t ytop
            ,
            x_bottom_right_trans - x_top_left_trans // int16_t width
            ,
            y_bottom_right_trans - y_top_left_trans // int16_t height
            ,
            i_bb.faceDetector // FaceDetectorType i_faceDetector
        );

        return square;
    }

    OFIQ_EXPORT size_t findLargestBoundingBox(const std::vector<OFIQ::BoundingBox>& faceRects)
    {
        if (faceRects.empty())
            throw OFIQError(
                OFIQ::ReturnCode::UnknownError,
                "Unable to find largest face in an empty list");

        // find largest bounding box (an iterator pointing to it)
        auto idxBiggestFace = std::max_element(
            faceRects.cbegin(),
            faceRects.cend(),
            [](const OFIQ::BoundingBox& a, const OFIQ::BoundingBox& b)
            { return a.width * a.height < b.width * b.height; });

        return (size_t)std::distance(faceRects.begin(), idxBiggestFace);
    }

    OFIQ_EXPORT OFIQ::Image MakeGreyImage(uint16_t width, uint16_t height)
    {
        std::shared_ptr<uint8_t> data{new uint8_t[width * height]};
        return {width, height, 8, data};
    }

    OFIQ_EXPORT cv::Mat copyToCvImage(const OFIQ::Image& sourceImage, bool asGrayImage)
    {
        bool isRGB = sourceImage.depth == 24;

        cv::Mat cvImage(sourceImage.height, sourceImage.width, isRGB ? CV_8UC3 : CV_8UC1);

        memcpy(cvImage.data, (char*)sourceImage.data.get(), sourceImage.size());

        if (!isRGB && !asGrayImage)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_GRAY2BGR);
        else if (isRGB && !asGrayImage)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_RGB2BGR);
        else if (isRGB && asGrayImage)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_RGB2GRAY);
        else if (!isRGB && asGrayImage)
        {
            // Then no work is necessary
        }

        return cvImage;
    }

    OFIQ_EXPORT cv::Mat alignImage(
        const OFIQ::Image& faceImage,
        const OFIQ::FaceLandmarks& faceLandmarks,
        OFIQ::FaceLandmarks& alignedFaceLandmarks,
        cv::Mat& transformationMatrix)
    {
        cv::Mat cvImage = copyToCvImage(faceImage);
        int nose, leftMouth, rightMouth;
        std::vector<cv::Point2f> landmarks, alignedLandmarks;
        switch (faceLandmarks.type)
        {
        case OFIQ::LandmarkType::LM_98:
        {
            nose = 54;
            leftMouth = 82;
            rightMouth = 76;
            int num_landmarks = 98;
            landmarks.reserve(num_landmarks);
            alignedLandmarks.reserve(num_landmarks);
            break;
        }
        default:
            throw std::invalid_argument("Unknown LandmarkType");
        }
        Point2f leftEyeCenter, rightEyeCenter;
        calculateEyeCenter(faceLandmarks, leftEyeCenter, rightEyeCenter);
        cv::Mat srcPoints = cv::Mat::zeros(5, 2, CV_32F);
        srcPoints.at<float>(0, 0) = leftEyeCenter.x;
        srcPoints.at<float>(0, 1) = leftEyeCenter.y;
        srcPoints.at<float>(1, 0) = rightEyeCenter.x;
        srcPoints.at<float>(1, 1) = rightEyeCenter.y;
        OFIQ::LandmarkPoint point = faceLandmarks.landmarks[nose];
        srcPoints.at<float>(2, 0) = point.x;
        srcPoints.at<float>(2, 1) = point.y;
        point = faceLandmarks.landmarks[rightMouth];
        srcPoints.at<float>(3, 0) = point.x;
        srcPoints.at<float>(3, 1) = point.y;
        point = faceLandmarks.landmarks[leftMouth];
        srcPoints.at<float>(4, 0) = point.x;
        srcPoints.at<float>(4, 1) = point.y;
        // define reference points
        float refData[10] = {251, 272, 364, 272, 308, 336, 262, 402, 355, 402};
        cv::Mat refPoints = cv::Mat(5, 2, CV_32F, refData);
        // calculate transformation matrix and warp image
        transformationMatrix = cv::estimateAffinePartial2D(srcPoints, refPoints, {}, cv::LMEDS);
        cv::Mat alignedImage;
        // cv::Mat alignedLandmarks = cv::Mat::zeros(landmarks.size(), CV_16UC1);;
        cv::warpAffine(cvImage, alignedImage, transformationMatrix, cv::Size(616, 616));
        for (int i = 0; i < faceLandmarks.landmarks.size(); i++)
        {
            OFIQ::LandmarkPoint landmark = faceLandmarks.landmarks[i];
            cv::Point point{landmark.x, landmark.y};
            landmarks.push_back(point);
        }
        cv::transform(landmarks, alignedLandmarks, transformationMatrix);
        for (auto& p : alignedLandmarks)
        {
            OFIQ::LandmarkPoint landmark;
            landmark.x = round(p.x);
            landmark.y = round(p.y);
            alignedFaceLandmarks.landmarks.push_back(landmark);
        }
        return alignedImage;
    }

    OFIQ_EXPORT void calculateEyeCenter(
        const OFIQ::FaceLandmarks& faceLandmarks, Point2f& leftEyeCenter, Point2f& rightEyeCenter)
    {
        auto leftEyePoints = PartExtractor::getFacePart(faceLandmarks, FaceParts::LEFT_EYE_CORNERS);
        auto rightEyePoints =
            PartExtractor::getFacePart(faceLandmarks, FaceParts::RIGHT_EYE_CORNERS);
        assert(leftEyePoints.size() == 2 && rightEyePoints.size() == 2);
        leftEyeCenter.x = round(leftEyePoints[0].x + 0.5 * (leftEyePoints[1].x - leftEyePoints[0].x));
        leftEyeCenter.y = round(leftEyePoints[0].y + 0.5 * (leftEyePoints[1].y - leftEyePoints[0].y));
        rightEyeCenter.x = round(rightEyePoints[0].x + 0.5 * (rightEyePoints[1].x - rightEyePoints[0].x));
        rightEyeCenter.y = round(rightEyePoints[0].y + 0.5 * (rightEyePoints[1].y - rightEyePoints[0].y));
    }

    OFIQ_EXPORT float tmetric(const OFIQ::FaceLandmarks& faceLandmarks)
    {
        Point2f leftEyeCenter, rightEyeCenter;
        calculateEyeCenter(faceLandmarks, leftEyeCenter, rightEyeCenter);
        OFIQ::Landmarks chinLandmarks = PartExtractor::getFacePart(faceLandmarks, FaceParts::CHIN);
        cv::Point2f eyeMidpoint((leftEyeCenter.x + rightEyeCenter.x) / 2.0, (leftEyeCenter.y + rightEyeCenter.y) / 2.0);
        cv::Point2f chin(chinLandmarks[0].x, chinLandmarks[0].y);

        return cv::norm(chin - eyeMidpoint);
    }

    OFIQ_EXPORT void rotationMatrixToEulerAngles(const cv::Mat& R, std::vector<double>& angles)
    {
        double& y = angles[0];
        double& x = angles[1];
        double& z = angles[2];
        double threshold = 0.9975;
        if (R.at<double>(2, 0) > threshold)
        {
            z = 0;
            x = -M_PI / 2;
            y = atan2(-R.at<double>(0, 1), -R.at<double>(0, 2));
        }
        else if (R.at<double>(2, 0) < -threshold)
        {
            z = 0;
            x = M_PI / 2;
            y = atan2(R.at<double>(0, 1), R.at<double>(0, 2));
        }
        else
        {
            x = -asin(R.at<double>(2, 0));
            y = atan2(R.at<double>(2, 1) / cos(x), R.at<double>(2, 2) / cos(x));
            z = atan2(R.at<double>(1, 0) / cos(x), R.at<double>(0, 0) / cos(x));
        }
        x = (x * 180) / M_PI;
        y = (y * 180) / M_PI;
        z = (z * 180) / M_PI;
    }
}
