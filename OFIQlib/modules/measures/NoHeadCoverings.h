/**
 * @file NoHeadCoverings.h
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
 * @brief Provides a class implementing the no head covering measure.
 * @author OFIQ development team
 */
#pragma once

#include "Measure.h"
#include "segmentations.h"

/**
 * @brief Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implementation of the no head covering measure.
     * @details The face parsing pre-processing assigns all pixels
     * on the aligned image to one class each encoded by a value between
     * 0 and 19 (inclusively). The values 16 and 18 encode the class
     * <i>clothing</i> and <i>head covering</i>, respectively. Assessment
     * of no head covering is done on the base of counting all pixels classified
     * as clothing and head covering on the upper part of the aligned facial
     * image and dividing it by the number of all pixels in the aligned image.
     * The ratio is the native quality score. If it exceeds a configurable
     * threshold, a quality the quality component value 0 is used; otherwise, 
     * if the ratio is below (or equals) the threshold, a quality of 100 is used.
     * @see \link OFIQ_LIB::modules::segmentations::FaceParsing FaceParsing\endlink
     */
    class NoHeadCoverings : public Measure
    {
    public:
        /**
         * @brief Constructor
         * @details The configuration object can optionally configure the threshold 
         * using the <code>params.measures.NoHeadCoverings.threshold</code> key which
         * is 0.02 by default. 
         * @param configuration Configuration object from which measure-related
         * configuration is read.
         * @param session Session object containing the original facial image
         * and pre-processing results
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method
         */
        NoHeadCoverings(
            const Configuration& configuration,
            Session& session);

        /**
         * @brief Assesses no head covering.
         * @details The face parsing pre-processing assigns all pixels
         * on the aligned image to one class each encoded by a value between
         * 0 and 19 (inclusively). The values 16 and 18 encode the class
         * <i>clothing</i> and <i>head covering</i>, respectively. Assessment
         * of no head covering is done on the base of counting all pixels classified
         * as clothing and head covering on the upper part of the aligned facial
         * image and dividing it by the number of all pixels in the aligned image.
         * The ratio is the native quality score. If it exceeds a configurable
         * threshold, a quality the quality component value 0 is used; otherwise,
         * if the ratio is below (or equals) the threshold, a quality of 100 is used.
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
         * OFIQImpl::performPreprocessing()\endlink method.
         * @see \link OFIQ_LIB::modules::segmentations::FaceParsing FaceParsing\endlink
         */
        void Execute(OFIQ_LIB::Session & session) override;

    private:
        /**
         * @brief Threshold
         * @details If the native quality score (number of pixels classified as
         * head covering divided by the number of total number of pixels in the aligned image),
         * is below (or equals) this threshold, then a quality of 100 (best) is used; 
         * otherwise, a quality of 0 is used.
         */
        double threshold;
    };
}
