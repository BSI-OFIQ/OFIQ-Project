/**
 * @file FaceMeasures.h
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
 * @brief Provides a class implementing two luminance measures.
 * @author OFIQ development team
 */
#pragma once

#include "ofiq_lib.h"
#include "PartExtractor.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Provides implementations related to facial landmarks. 
 */
namespace OFIQ_LIB::modules::landmarks
{
    /**
     * @brief Provides static functions doing computations with landmarks. 
     */
    class FaceMeasures
    {
    public:
        /**
         * @brief Constructor is deleted to avoid instantiations of this class. 
         */
        FaceMeasures() = delete;

        /**
         * @brief Computes the inter-eye distance based on the specified 
         * facial landmarks and yaw angle.
         * @details If \f$(x_0,y_0)\f$ and \f$(x_1,y_1)\f$ are the left and
         * right eye centres, then the following inter-eye distance is computed.
         * \f[
         *  \mathrm{IED}=\|(x_0,y_0)-(x_1,y_1)\|_2\cdot\frac{1}{\cos(\alpha)}
         * \f]
         * where
         * \f[
         *  \alpha=\pi\cdot\mathrm{yaw} / 180
         * \f]
         * is the secant of the yaw angle.
         * @param faceLandmarks Facial landmarks
         * @param yaw Yaw angle in degree
         * @return The inter-eye distance
         */
        static double InterEyeDistance(const OFIQ::FaceLandmarks& faceLandmarks, double yaw);

        /**
         * @brief Creates a binary image of specified dimension and masks all pixels inside or on the convex hull.
         * @details All pixels on or inside the convex hull of the landmarks are set to 1; all other
         * pixels are set to 0.
         * @param faceLandmarks Facial landmarks object
         * @param height Height of the mask image
         * @param width Width of the mask image
         * @param alpha Should be 0; different values have only be used for NIST submissions.
         * @return Mask image
         */
        static cv::Mat GetFaceMask
        (const OFIQ::FaceLandmarks& faceLandmarks, const int height, const int width, 
         const float alpha = 0);

        /**
         * @brief Convenience method for computing the Euclidean distance between two landmark points.
         * @param a First landmark point
         * @param b Second landmark point
         * @return Euclidean distance between <code>a</code> and <code>b</code>.
         */
        static double GetDistance(const OFIQ::LandmarkPoint& a, const OFIQ::LandmarkPoint& b);

        /**
         * @brief Convenience method computing the Euclidean distance between two landmark points.
         * @param pair The two landmark points stored in the member <code>pair.Lower</code> and 
         * <code>pair.Upper</code>.
         * @return Euclidean distance.
         */
        static double GetDistance(const LandmarkPair& pair)
        {
            return GetDistance(pair.Lower, pair.Upper);
        }

        /**
         * @brief Computes the center point of the specified landmarks.
         * @param landmarks Facial landmarks
         * @return Center point of the landmarks.
         */
        static OFIQ::LandmarkPoint GetMiddle(const OFIQ::Landmarks& landmarks);

        /**
         * @brief Computes the point in between two landmark points.
         * @param pair Pair of landmark points.
         * @return Point between the two landmark points.
         */
        static OFIQ::LandmarkPoint GetMiddle(const LandmarkPair& pair)
        {
            return GetMiddle(OFIQ::Landmarks{pair.Lower, pair.Upper});
        }

        /**
         * @brief Computes the center of the specified landmark points.
         * @details This is a convenience method to compute the center if
         * the landmarks are available as a vector of landmark pairs.
         * @param pairs List of landmark pairs.
         * @return Center of the landmark points.
         */
        static OFIQ::LandmarkPoint GetMiddle(const std::vector<LandmarkPair>& pairs)
        {
            std::vector<OFIQ::LandmarkPoint> points;
            for (auto pair : pairs)
            {
                points.push_back(GetMiddle(pair));
            }
            return GetMiddle(points);
        }

        /**
         * @brief Returns this maximum of face pairs from landmarks 
         * corresponding to the specified face part.
         * @details Face parts (such as mouth) consist of landmarks 
         * that have a mate. For example, the lower lip point may
         * correspond to the upper lip point. For such face parts,
         * the maximal separation (e.g., useful for detecting the
         * mouth closeness or eyes openness) can be computed.
         * @param landmarks Facial landmarks
         * @param facePart Face part
         * @return Maxim of face pairs
         */
        static double GetMaxPairDistance
        (const OFIQ::FaceLandmarks& landmarks, 
         landmarks::FaceParts facePart);
    };
}