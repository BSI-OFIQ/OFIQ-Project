/**
 * @file NaturalColour.h
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
 * @brief Provides a class implementing the natural colour measure.
 * @author OFIQ development team
 */
#pragma once

#include "landmarks.h"
#include "Measure.h"

 /**
  * @brief Provides measures implemented in OFIQ.
  */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implementation of the natural colour measure.
     * @details Assessment of the naturalness of the colour based on
     * the conversion of the RGB presentation of the image to
     * the CIELAB colour space.
     */
    class NaturalColour : public Measure
    {
    public:
        /**
         * @brief Constructor
         * @param configuration Configuration object from which measure-related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method
         */
        NaturalColour(
            const Configuration& configuration,
            Session& session);

        /**
         * @brief Assesses natural colourness
         * @details Assessment of the naturalness of the colour based on
         * the conversion of the RGB presentation of the image to
         * the CIELAB colour space.
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         */
        void Execute(OFIQ_LIB::Session & session) override;

    private:
        /**
         * @brief Creates a mask image from the convex full of the specified landmarks.
         * @param landmarks Facial landmarks.
         * @param cvImage The mask image returned has the same dimension as <code>cvImage</code>.
         * @return Mask image
         */
        cv::Mat CreateMaskedImage(const OFIQ::FaceLandmarks& landmarks, const cv::Mat& cvImage);

        /**
         * @brief Extracts two rectangular regions from an image and returns its concatenation.
         * @param maskedImage The input image from which the two regions are extracted.
         * @param leftRegionOfInterest First region
         * @param rightRegionOfInterest Second region
         * @return Concatenation if the requested regions; the first columns correspond to
         * <code>rightRegionOfInterest</code> and the last columns correspond to 
         * <code>leftRegionOfInterest</code>.
         * @attention An error occurs if the height of the two requested regions differ.
         */
        cv::Mat ReduceImageToRegionOfInterest
        (const cv::Mat maskedImage, 
         const cv::Rect& leftRegionOfInterest, 
         const cv::Rect& rightRegionOfInterest);
        
        /**
         * @brief Combines two CIELAB values a* and b* to computed
         * the native quality score.
         * @details If \f$a^*\f$ and \f$b^*\f$ are both larger than or equals to 0, then the following 
         * formula is applied
         * \f[
         *  D = \sqrt{\max(\max(0,5-a^*),\max(0,a^*-25))^2+\max(\max(0,5-b^*),\max(b^*-35))^2}
         * \f]
         * and \f$D\f$ is returned; otherwise, the value 100 is returned.
         * @param meanChannelA The CIELAB value \f$a^*\f$ input value.
         * @param meanChannelB The CIELAB value \f$b^*\f$ input value.
         * @return Native quality score
         */
        double CalculateScore(double meanChannelA, double meanChannelB);
    };
}