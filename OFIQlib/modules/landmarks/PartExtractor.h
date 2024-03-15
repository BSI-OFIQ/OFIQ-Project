/**
 * @file PartExtractor.h
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
 * @brief Provides helper class for face landmark handling 
 * @author OFIQ development team
 */
#pragma once

#include "ofiq_lib.h"
#include "FaceParts.h"

/**
 * @brief Provides implementations for computations with landmarks. 
 */
namespace OFIQ_LIB::modules::landmarks
{

    /**
     * @brief forward declaration
     * 
     */
    enum class FaceParts;

    /**
     * @brief Data container for storing pairs of landmarks.
     * @details in some computation special landmarks are bound together via the LandmarkPair struct. 
     */
    struct LandmarkPair
    {
        /**
         * @brief First Landmark
         * 
         */
        OFIQ::LandmarkPoint Upper;

        /**
         * @brief second landmark
         * 
         */
        OFIQ::LandmarkPoint Lower;

        /**
         * @brief Parameterized constructor
         * 
         * @param[in] upper LandmarkPoint of first landmark.
         * @param[in] lower LandmarkPoint of second landmark.
         */
        LandmarkPair(OFIQ::LandmarkPoint upper, OFIQ::LandmarkPoint lower) : Upper{upper}, Lower{lower}
        {
        }
    };

    /**
     * @brief Class that provides helper methods for the administration of landmarks.
     * 
     */
    class PartExtractor
    {
    public:
        /**
         * @brief Extract the landmarks that correspondend to the 
         * requested face part out of a set of provided landmarks.
         * 
         * @param[in] faceLandmarks Landmarks to be filtered.
         * @param part Face part of interest.
         * @return OFIQ::Landmarks Filtered landmarks that belong to the requested face part.
         */
        static OFIQ::Landmarks getFacePart(const OFIQ::FaceLandmarks& faceLandmarks, FaceParts part);

        /**
         * @brief Get LandmarkPairs for a face part.
         * @details LandmarkPairs might be used to compute a distance between upper and lower landmark.
         * 
         * @param faceLandmarks Set of face landmarks.
         * @param part Face part of interest.
         * @return std::vector<LandmarkPair> 
         */
        static std::vector<LandmarkPair> getPairsForPart(const OFIQ::FaceLandmarks& faceLandmarks, FaceParts part);
    };
}