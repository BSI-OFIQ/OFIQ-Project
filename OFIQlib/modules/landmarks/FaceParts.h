/**
 * @file FaceParts.h
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
 * @brief PRovides several helper classes, enums and functions used in the OFIQ framework.
 * @author OFIQ development team
 */

#pragma once

 /**
  * @brief Provides implementations of a landmark extractors.
  */
namespace OFIQ_LIB::modules::landmarks
{
    /**
     * @brief Enumeration of facial landmark parts 
     */
    enum class FaceParts
    {
        /** left as seen on the image, it's the persons right eye */
        LEFT_EYE,
        /** right as seen on the image, it's the persons left eye */
        RIGHT_EYE,
        /** two landmarks - outer, inner corner */
        LEFT_EYE_CORNERS,
        /** two landmarks - outer, inner corner */
        RIGHT_EYE_CORNERS,
        /** outer landmarks of mouth */
        MOUTH_OUTER,
        /** inner landmarks of mouth */
        MOUTH_INNER,
        /** contour of the face */
        FACE_CONTOUR,
        /** center of the mouth */
        MOUTH_CENTER,
        /** chin */
        CHIN,
        /** nosetip */
        NOSETIP,
        /** forehead */
        FOREHEAD
    };

    /**
     * @brief Type definition of a landmark index. 
     */
    using LandmarkId = int;

    /**
     * @brief Type definition of a list of landmark indices.
     */
    using LandmarkIds = std::vector<LandmarkId>;

    /**
     * @brief Type definition of a face map to
     * access landmark indices for a queried face part.
     */
    using FaceMap = std::map<FaceParts, LandmarkIds>;

    /**
     * @brief Type definition for a pair of landmark index. 
     */
    using LandmarkIdPair = std::array<LandmarkId, 2>;

    /**
     * @brief Type definition for a list of landmark index pairs. 
     */
    using LandmarkIdPairs = std::vector<LandmarkIdPair>;

    /**
     * @brief Structure defining pairs of landmark indices. 
     */
    using FacePairMap = std::map<FaceParts, LandmarkIdPairs>;
}