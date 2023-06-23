#include "FaceMeasures.h"
#include "FaceParts.h"
#include <math.h>
#include <opencv2/imgproc.hpp>

namespace FACEQA_LIB::modules::landmarks
{
    FACEQA::LandmarkPoint FaceMeasures::GetMiddle(const FACEQA::Landmarks& landmarks)
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

        FACEQA::LandmarkPoint point;
        if (count > 0)
        {
            point.x = static_cast<int16_t>(sumX / count);
            point.y = static_cast<int16_t>(sumY / count);
        }

        return point;
    }

    double FaceMeasures::GetDistance(const FACEQA::LandmarkPoint& a, const FACEQA::LandmarkPoint& b)
    {
        auto distanceX = a.x - b.x;
        auto distanceY = a.y - b.y;

        auto distanceSquared = 1.0 * distanceX * distanceX + 1.0 * distanceY * distanceY;

        return sqrt(distanceSquared);
    }

    double FaceMeasures::InterEyeDistance(const FACEQA::FaceLandmarks& faceLandmarks)
    {
        auto leftEyePoints = PartExtractor::getFacePart(faceLandmarks, FaceParts::LEFT_EYE_CORNERS);
        auto rightEyePoints =
            PartExtractor::getFacePart(faceLandmarks, FaceParts::RIGHT_EYE_CORNERS);

        auto leftCenter = GetMiddle(leftEyePoints);
        auto rightCenter = GetMiddle(rightEyePoints);

        return GetDistance(leftCenter, rightCenter);
    }

    static FACEQA::Image MakeGreyImage(uint16_t width, uint16_t height)
    {
        std::shared_ptr<uint8_t[]> data{new uint8_t[width * height]};
        return {width, height, 8, data};
    }

    FACEQA::Image FaceMeasures::GetFaceMask(
        const FACEQA::FaceLandmarks& faceLandmarks, const FACEQA::Image& faceImage)
    {
        std::vector<cv::Point2i> landmarkPoints;
        for (const auto& landmark : faceLandmarks.landmarks)
        {
            landmarkPoints.emplace_back(landmark.x, landmark.y);
        }
        std::vector<cv::Point2i> hullPoints;
        cv::convexHull(landmarkPoints, hullPoints);

        cv::Mat cvMaskImage(faceImage.height, faceImage.width, CV_8UC1, cv::Scalar(0));
        cv::Scalar maskColor = 255;
        cv::fillConvexPoly(cvMaskImage, hullPoints, maskColor);

        FACEQA::Image faceMaskImage = MakeGreyImage(faceImage.width, faceImage.height);
        memcpy(faceMaskImage.data.get(), cvMaskImage.data, faceMaskImage.size());

        return faceMaskImage;
    }

    double FaceMeasures::GetMaxPairDistance(
        const FACEQA::FaceLandmarks& landmarks, landmarks::FaceParts facePart)
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