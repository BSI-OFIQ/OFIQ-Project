/**
 * @file IlluminationUniformity.cpp
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

#include "IlluminationUniformity.h"
#include "OFIQError.h"
#include "FaceMeasures.h"
#include "image_utils.h"
#include "FaceParts.h"

using PartExtractor = OFIQ_LIB::modules::landmarks::PartExtractor;
using FaceParts = OFIQ_LIB::modules::landmarks::FaceParts;
using FaceMeasures = OFIQ_LIB::modules::landmarks::FaceMeasures;

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::IlluminationUniformity;

    IlluminationUniformity::IlluminationUniformity(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
    }

    void IlluminationUniformity::Execute(OFIQ_LIB::Session & session)
    {
        auto landmarks = session.getAlignedFaceLandmarks();
        cv::Mat alignedImage = session.getAlignedFace();

        // Find and segment the face region
        cv::Mat mask = session.getAlignedFaceLandmarkedRegion() * 255;
        cv::Mat faceSegmentation;
        cv::bitwise_and(alignedImage, alignedImage, faceSegmentation, mask);

        // Recover the image luminance from RGB
        auto luminanceImage = GetLuminanceImageFromBGR(faceSegmentation);

        // Compute the RMZ and LMZ of the face
        OFIQ::LandmarkPoint leftEyeCenter, rightEyeCenter;
        double interEyeDistance, eyeMouthDistance;
        CalculateReferencePoints(landmarks, leftEyeCenter, rightEyeCenter, interEyeDistance, eyeMouthDistance);
        cv::Rect leftRegionOfInterest;
        cv::Rect rightRegionOfInterest;
        CalculateRegionOfInterest(leftRegionOfInterest, rightRegionOfInterest, leftEyeCenter, rightEyeCenter, interEyeDistance, eyeMouthDistance);
        auto leftRegion = luminanceImage(leftRegionOfInterest);
        auto rightRegion = luminanceImage(rightRegionOfInterest);

        // Compute the normalized luminance histograms for RMZ and LMZ
        cv::Mat1f leftHistogram, rightHistogram;
        GetNormalizedHistogram(leftRegion, cv::Mat(), leftHistogram);
        GetNormalizedHistogram(rightRegion, cv::Mat(), rightHistogram);

        // Get element-wise minimum of the normalized histograms
        cv::Mat minHistogram = cv::min(leftHistogram, rightHistogram);

        // Get the sum of the minimums
        double rawScore = cv::sum(minHistogram).val[0];

        double scalarScore = round(100 * (std::pow(rawScore, 0.3)));
        session.assessment().qAssessments[qualityMeasure] = { rawScore, scalarScore, OFIQ::QualityMeasureReturnCode::Success };
    }
}