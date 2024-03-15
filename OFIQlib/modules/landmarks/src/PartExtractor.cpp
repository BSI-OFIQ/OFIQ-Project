/**
 * @file PartExtractor.cpp
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

#include "PartExtractor.h"
#include "adnet_FaceMap.h"

namespace OFIQ_LIB::modules::landmarks
{
    OFIQ::Landmarks
        PartExtractor::getFacePart(const OFIQ::FaceLandmarks& faceLandmarks, FaceParts part)
    {
        const FaceMap* FaceMap;
        OFIQ::Landmarks selectedLandmarks;

        switch (faceLandmarks.type)
        {
        case OFIQ::LandmarkType::LM_98:
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
        PartExtractor::getPairsForPart(const OFIQ::FaceLandmarks& faceLandmarks, FaceParts part)
    {
        const FacePairMap* FacePairMap;
        std::vector<LandmarkPair> selectedLandmarkPairs;

        switch (faceLandmarks.type)
        {
        case OFIQ::LandmarkType::LM_98:
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