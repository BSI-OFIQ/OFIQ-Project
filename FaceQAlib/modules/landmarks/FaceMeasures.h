#pragma once

#include "faceqa_lib.h"
#include "PartExtractor.h"

namespace FACEQA_LIB::modules::landmarks
{
    class FaceMeasures
    {
    public:
        static double InterEyeDistance(const FACEQA::FaceLandmarks& faceLandmarks);
        static FACEQA::Image GetFaceMask(const FACEQA::FaceLandmarks& faceLandmarks, const FACEQA::Image& faceImage);
        static double GetDistance(const FACEQA::LandmarkPoint& a, const FACEQA::LandmarkPoint& b);

        static double GetDistance(const LandmarkPair& pair)
        {
            return GetDistance(pair.Lower, pair.Upper);
        }

        static FACEQA::LandmarkPoint GetMiddle(const FACEQA::Landmarks& landmarks);

        static FACEQA::LandmarkPoint GetMiddle(const LandmarkPair& pair)
        {
            return GetMiddle(FACEQA::Landmarks{pair.Lower, pair.Upper});
        }

        static FACEQA::LandmarkPoint GetMiddle(const std::vector<LandmarkPair>& pairs)
        {
            std::vector<FACEQA::LandmarkPoint> points;
            for (auto pair : pairs)
            {
                points.push_back(GetMiddle(pair));
            }
            return GetMiddle(points);
        }

        static double GetMaxPairDistance(const FACEQA::FaceLandmarks& landmarks, landmarks::FaceParts facePart);
    };
}