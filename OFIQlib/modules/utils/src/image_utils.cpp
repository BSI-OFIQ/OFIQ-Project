/**
 * @file image_utils.cpp
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

#include "image_utils.h"
#include "landmarks.h"
#include "FaceMeasures.h"
#include "FaceParts.h"

using PartExtractor = OFIQ_LIB::modules::landmarks::PartExtractor;
using FaceParts = OFIQ_LIB::modules::landmarks::FaceParts;
using FaceMeasures = OFIQ_LIB::modules::landmarks::FaceMeasures;

namespace OFIQ_LIB
{

    double ColorConvert(double x)
    {
        if (x > 0.04045)
        {
            return pow( (x+0.055)/1.055 , 2.4);
        }
        
        return x / 12.92;
    }

    double Cubic(double x, double k, double eps)
    {
        if (x <= eps)
        {
            return ((k * x) + 16) / 116;
        }

        return std::cbrt(x);
    }

    void ConvertBGRToCIELAB(const cv::Mat& rgbImage, double& a, double& b)
    {
        double k = 24289 / 27.0;
        double eps = 216 / 24389.0;

        std::vector<cv::Mat> channels;
        cv::split(rgbImage, channels);
        double R = mean(channels[2])[0] / 255.0;
        double G = mean(channels[1])[0] / 255.0;
        double B = mean(channels[0])[0] / 255.0;

        double R_L = ColorConvert(R);
        double G_L = ColorConvert(G);
        double B_L = ColorConvert(B);

        double X = R_L * 0.43605 + G_L * 0.38508 + B_L * 0.14309;
        double Y = R_L * 0.22249 + G_L * 0.71689 + B_L * 0.06062;
        double Z = R_L * 0.01393 + G_L * 0.09710 + B_L * 0.71419;

        double X_R = X / 0.964221;
        double Y_R = Y;
        double Z_R = Z / 0.825211;

        double F_X = Cubic(X_R, k, eps);
        double F_Y = Cubic(Y_R, k, eps);
        double F_Z = Cubic(Z_R, k, eps);

        a = 500.0 * (F_X - F_Y);
        b = 200.0 * (F_Y - F_Z);
    }

	cv::Mat GetLuminanceImageFromBGR(const cv::Mat& bgrImage)
	{
        cv::Mat L = cv::Mat::zeros(bgrImage.rows, bgrImage.cols, CV_8U);

        for (int i = 0; i < L.rows; i++)
        {
            for (int j = 0; j < L.cols; j++)
            {
                auto & pixel = bgrImage.at<cv::Vec3b>(i,j);

                double r_norm = pixel[2] / 255.0;
                double g_norm = pixel[1] / 255.0;
                double b_norm = pixel[0] / 255.0;

                double r_lum = ColorConvert(r_norm);
                double g_lum = ColorConvert(g_norm);
                double b_lum = ColorConvert(b_norm);

                double y = 0.2126 * r_lum + 0.7152 * g_lum + 0.0722 * b_lum;
                L.data[L.cols*i + j] = (uint8_t)floor(y*255+0.5);
            }
        }

        return L;
	}

    void CalculateReferencePoints(const OFIQ::FaceLandmarks& landmarks, OFIQ::LandmarkPoint& leftEyeCenter, OFIQ::LandmarkPoint& rightEyeCenter,
        double& interEyeDistance, double& eyeMouthDistance)
    {
        auto leftEyePoints = PartExtractor::getFacePart(landmarks, FaceParts::LEFT_EYE_CORNERS);
        auto rightEyePoints = PartExtractor::getFacePart(landmarks, FaceParts::RIGHT_EYE_CORNERS);
        leftEyeCenter = FaceMeasures::GetMiddle(leftEyePoints);
        rightEyeCenter = FaceMeasures::GetMiddle(rightEyePoints);

        interEyeDistance = FaceMeasures::GetDistance(leftEyeCenter, rightEyeCenter);
        auto eyeMidPoint =
            FaceMeasures::GetMiddle(OFIQ::Landmarks{ leftEyeCenter, rightEyeCenter });

        auto mouthCenter = FaceMeasures::GetMiddle(
            PartExtractor::getPairsForPart(landmarks, FaceParts::MOUTH_CENTER));
        eyeMouthDistance = FaceMeasures::GetDistance(eyeMidPoint, mouthCenter);
    }

    void CalculateRegionOfInterest(cv::Rect& leftRegionOfInterest, cv::Rect& rightRegionOfInterest, const OFIQ::LandmarkPoint& leftEyeCenter, const OFIQ::LandmarkPoint& rightEyeCenter,
        const double interEyeDistance, const double eyeMouthDistance)
    {
        auto zoneSize = static_cast<int>(interEyeDistance * 0.3);
        rightRegionOfInterest.x = rightEyeCenter.x - zoneSize;
        rightRegionOfInterest.y = rightEyeCenter.y + static_cast<int>(eyeMouthDistance / 2);
        rightRegionOfInterest.height = rightRegionOfInterest.width = zoneSize;

        leftRegionOfInterest.x = leftEyeCenter.x;
        leftRegionOfInterest.y = leftEyeCenter.y + static_cast<int>(eyeMouthDistance / 2);
        leftRegionOfInterest.height = leftRegionOfInterest.width = zoneSize;
    }

    void GetNormalizedHistogram(const cv::Mat& luminanceImage, const cv::Mat& maskImage, cv::Mat1f& histogram)
    {
        int histSize = 256;
        std::vector<float> range = { 0, 256 };

        cv::calcHist(std::vector{ luminanceImage }, { 0 }, maskImage, histogram, { histSize }, range);

        auto pixelsInHistogram = cv::sum(histogram).val[0];
        histogram = histogram / pixelsInHistogram;
    }

    double CalculateExposure(const Session& session, const ExposureRange& exposureRange)
    {
        double quality = 0.0;

        cv::Mat faceOcclusionMask = session.getFaceOcclusionSegmentationImage();
        cv::Mat faceMask = session.getAlignedFaceLandmarkedRegion();
        auto alignedImage = session.getAlignedFace();

        cv::Mat maskedImage;
        cv::bitwise_and(faceMask, faceOcclusionMask, maskedImage);
        
        auto luminanceImage = GetLuminanceImageFromBGR(alignedImage);

        quality = ComputeBrightnessAspect(
            luminanceImage, maskedImage, exposureRange
        );

        return quality;
    }


    double ComputeBrightnessAspect(
        const cv::Mat& luminanceImage, const cv::Mat& maskImage, const ExposureRange& exposureRange)
    {
        int histSize = 256;
        std::vector<float> range = {0, 256};
        cv::Mat1f histogram;

        cv::calcHist(std::vector{luminanceImage}, {0}, maskImage, histogram, {histSize}, range);

        auto pixelsInHistogram = cv::sum(histogram).val[0];
        double rawScore = 0;
        for (int i = exposureRange[0]; i <= exposureRange[1]; i++)
        {
            rawScore += histogram.at<float>(i) / pixelsInHistogram;
        }

        return rawScore;
    }
}