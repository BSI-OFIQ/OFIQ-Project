/**
 * @file FaceMeasure.cpp
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

#include "FaceMeasures.h"
#include "FaceParts.h"
#include <math.h>
#include <unordered_set>
#include <opencv2/imgproc.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

namespace OFIQ_LIB::modules::landmarks
{
    OFIQ::LandmarkPoint FaceMeasures::GetMiddle(const OFIQ::Landmarks& landmarks)
    {
        int32_t sumX = 0;
        int32_t sumY = 0;
        size_t count = 0;

        for (const auto& item : landmarks)
        {
            sumX += item.x;
            sumY += item.y;
            count++;
        }

        OFIQ::LandmarkPoint point;
        if (count > 0)
        {
            point.x = round(sumX / (float)count);
            point.y = round(sumY / (float)count);
        }

        return point;
    }

    double FaceMeasures::GetDistance(const OFIQ::LandmarkPoint& a, const OFIQ::LandmarkPoint& b)
    {
        auto distanceX = a.x - b.x;
        auto distanceY = a.y - b.y;

        auto distanceSquared = 1.0 * distanceX * distanceX + 1.0 * distanceY * distanceY;

        return sqrt(distanceSquared);
    }

    double FaceMeasures::InterEyeDistance(const OFIQ::FaceLandmarks& faceLandmarks, double yaw)
    {
        auto leftEyePoints = PartExtractor::getFacePart(faceLandmarks, FaceParts::LEFT_EYE_CORNERS);
        auto rightEyePoints =
            PartExtractor::getFacePart(faceLandmarks, FaceParts::RIGHT_EYE_CORNERS);

        auto leftCenter = GetMiddle(leftEyePoints);
        auto rightCenter = GetMiddle(rightEyePoints);

        double distance = GetDistance(leftCenter, rightCenter);
        distance *= 1 / (cos(yaw * M_PI / 180.0));
        return distance;
    }


    cv::Mat FaceMeasures::GetFaceMask(
        const OFIQ::FaceLandmarks& faceLandmarks, const int height, const int width, const float alpha)
    {
        auto landmarks = faceLandmarks.landmarks;    
        std::vector<cv::Point2i> landmarkPoints;
        for (const auto& landmark : landmarks)
        {
            landmarkPoints.emplace_back(landmark.x, landmark.y);
        }
        // optional: expand convex hull
        if (alpha > 0)
        {
            std::vector<cv::Point2i> contour;
            cv::Point2f eyesMidpoint, chin;
            OFIQ::Landmarks eyeCorners = PartExtractor::getFacePart(faceLandmarks, FaceParts::LEFT_EYE_CORNERS);
            OFIQ::Landmarks rightEyeCorners = PartExtractor::getFacePart(faceLandmarks, FaceParts::RIGHT_EYE_CORNERS);
            eyeCorners.insert(eyeCorners.end(), rightEyeCorners.begin(), rightEyeCorners.end());
            for (int i = 0; i < eyeCorners.size(); i++)
            {
                eyesMidpoint += cv::Point2f(eyeCorners[i].x, eyeCorners[i].y);
            }
            eyesMidpoint /= (int)eyeCorners.size();
            switch (faceLandmarks.type)
            {
            case OFIQ::LandmarkType::LM_98:
            {
                int chinIndex = 16;
                chin = landmarkPoints[chinIndex];
                std::vector<int> contourIndices = { 0, 7, 25, 32 };
                for (int i = 0; i < contourIndices.size(); i++)
                {
                    contour.push_back(landmarkPoints[contourIndices[i]]);
                }
                break;
            }
            default:
                throw std::invalid_argument("Unknown LandmarkType");
            }
            cv::Point2f chinMidpointVector = eyesMidpoint - chin;
            cv::Point2i topOfForehead{ (int)(eyesMidpoint.x + alpha * chinMidpointVector.x), (int)(eyesMidpoint.y + alpha * chinMidpointVector.y) };
            std::vector <cv::Point2i> ellipsePoints = contour;
            ellipsePoints.push_back(chin);
            ellipsePoints.push_back(topOfForehead);
            cv::RotatedRect ellipse = cv::fitEllipse(ellipsePoints);
            std::vector<cv::Point2i> polyPoints;
            cv::Point2i ellipseCenter{ (int)ellipse.center.x, (int)ellipse.center.y };
            cv::Size ellipseSize = { (int)(ellipse.size.width / 2), (int)(ellipse.size.height / 2) };
            // Get landmarks for the ellipse
            cv::ellipse2Poly(ellipseCenter, ellipseSize, (int)ellipse.angle, 0, 360, 10, polyPoints);
            // discard ellipse points which are not on forehead
            polyPoints.erase(std::remove_if(polyPoints.begin(), polyPoints.end(), [chin, chinMidpointVector](cv::Point2f p)
            {
                return (p - chin).dot(chinMidpointVector) <= 1.1 * chinMidpointVector.dot(chinMidpointVector);
            }), polyPoints.end());
            landmarkPoints.reserve(landmarkPoints.size() + polyPoints.size());
            landmarkPoints.insert(landmarkPoints.end(), polyPoints.begin(), polyPoints.end());
        }

        std::vector<cv::Point2i> hullPoints;
        cv::convexHull(landmarkPoints, hullPoints);
        cv::Rect rect = cv::boundingRect(hullPoints);
        int b = (int)(rect.y - rect.height * 0.05);
        int d = (int)(rect.y + rect.height * 1.05);
        int a = (int)(rect.x + rect.width / 2.0 - (d - b) / 2.0);
        int c = (int)(rect.x + rect.width / 2.0 + (d - b) / 2.0);
        // relative landmarks on cropped image
        int imgSize = 224;
        for (auto& p : hullPoints)
        {
            cv::Point2f cropped = p;
            cropped = (cropped - cv::Point2f(a, b)) / (d - b) * imgSize;
            p.x = (int)cropped.x;
            p.y = (int)cropped.y;
        }
        // generate mask of convex hull
        cv::Mat mask = cv::Mat::zeros(cv::Size(imgSize, imgSize), CV_8UC1);
        cv::Scalar maskColor = 1;
        cv::fillConvexPoly(mask, hullPoints, cv::Scalar(1));
        cv::Mat faceRegion = cv::Mat::zeros(cv::Size(width, height), CV_8UC1);
        cv::Mat maskRescaled;
        cv::resize(mask, maskRescaled, cv::Size(c - a, d - b), 0, 0, cv::INTER_NEAREST);
        int left = 0, top = 0, right = maskRescaled.cols, bottom = maskRescaled.rows;
        int an = a, bn = b, cn = c, dn = d;
        if (a < 0)
        {
            left -= a;
            an = 0;
        }
        if (c > width)
        {
            right = maskRescaled.cols - (c - width);
            cn = width;
        }
        if (b < 0)
        {
            top -= b;
            bn = 0;
        }
        if (d > height)
        {
            bottom = maskRescaled.rows - (d - height);
            dn = height;
        }
        cv::Mat crop = maskRescaled(cv::Range(top, bottom), cv::Range(left, right));
        crop.copyTo(faceRegion(cv::Range(bn, dn), cv::Range(an, cn)));
        return faceRegion;
    }

    double FaceMeasures::GetMaxPairDistance(
        const OFIQ::FaceLandmarks& landmarks, landmarks::FaceParts facePart)
    {
        double maxDistance = 0;

        auto pairs = landmarks::PartExtractor::getPairsForPart(landmarks, facePart);

        for (auto pair : pairs)
        {
            auto distance = landmarks::FaceMeasures::GetDistance(pair);
            maxDistance = std::max(maxDistance, distance);
        }

        return maxDistance;
    }
}