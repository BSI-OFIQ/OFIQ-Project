#include "PartExtractor.h"
#include "adnet_FaceMap.h"

namespace FACEQA_LIB::modules::landmarks
{
    FACEQA::Landmarks
        PartExtractor::getFacePart(const FACEQA::FaceLandmarks& faceLandmarks, FaceParts part)
    {
        const FaceMap* FaceMap;
        FACEQA::Landmarks selectedLandmarks;

        switch (faceLandmarks.type)
        {
        case FACEQA::LandmarkType::ADNet:
            FaceMap = &adnet::FaceMap;
            break;
        default:
            throw std::invalid_argument("Unknown LandmarkType");
        }

        auto iterator = FaceMap->find(part);
        if (iterator != FaceMap->end())
        {
            const auto& list = iterator->second;
            for (const auto& index : list)
            {
                selectedLandmarks.push_back(faceLandmarks.landmarks[index]);
            }
        }

        return selectedLandmarks;
    }

    std::vector<LandmarkPair>
        PartExtractor::getPairsForPart(const FACEQA::FaceLandmarks& faceLandmarks, FaceParts part)
    {
        const FacePairMap* FacePairMap;
        std::vector<LandmarkPair> selectedLandmarkPairs;

        switch (faceLandmarks.type)
        {
        case FACEQA::LandmarkType::ADNet:
            FacePairMap = &adnet::FacePairMap;
            break;
        default:
            throw std::invalid_argument("Unknown LandmarkType");
        }

        auto iterator = FacePairMap->find(part);
        if (iterator != FacePairMap->end())
        {
            const auto& list = iterator->second;
            for (const auto& index : list)
            {
                selectedLandmarkPairs.push_back(
                    {faceLandmarks.landmarks[index[0]], faceLandmarks.landmarks[index[1]]});
            }
        }

        return selectedLandmarkPairs;
    }
}