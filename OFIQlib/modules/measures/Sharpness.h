/**
 * @file Sharpness.h
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
 * @brief Provides a class implementing the sharpness measure.
 * @author OFIQ development team
 */
#pragma once

#include "Measure.h"

/**
 * @brief Provides measures implemented in OFIQ.
 */
namespace OFIQ_LIB::modules::measures
{
    /**
     * @brief Implemantation of the sharpness measure.
     * @details This quality component can be used to efficiently 
     * choose the better focused face portrait among several face samples 
     * of the same biometric capture subject. It should not be used to perform 
     * an absolute sharpness assessment if only one sample is available
     */
    class Sharpness : public Measure
    {
    public:
        /**
         * @brief Construct a new Sharpness object
         * @param configuration Configuration object from which measure-related configuration is read.
         * @param session Session object containing the original facial image and pre-processing results 
         * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method 
         */
        Sharpness(
            const Configuration& configuration,
            Session& session);

        /**
         * @brief Run  computation of the sharpness measure.
         * @param session Session object computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing() 
         * OFIQImpl::performPreprocessing()\endlink method.
         */
        void Execute(OFIQ_LIB::Session & session) override;

    private:

        /**
         * @brief Name of the random forest model, extracted from the configuration file.
         */
        std::string modelFile;

        /**
         * @brief Instance of the random forest model.
         * 
         */
        std::shared_ptr<cv::ml::RTrees> rtree;

        /**
         * @brief The sharpness measure can be computed on the aligned or the original image. useAligned set to true will 
         * run the computation on the aligned image. The member is read from the configuration file.
         * 
         */
        bool useAligned;

        /**
         * @brief For faceRegionAlpha = 0, the algorithm uses the inner face region. 
         * For faceRegionAlpha = 0.85, the algorithm uses the extended face region as specified for the FaceOcclusionin FRVT Quality.
         * 
         */
        double faceRegionAlpha;

        /**
         * @brief This member stores the number of trees used for the random forest. Internal use only.
         * 
         */
        int numTrees;

        /**
         * @brief Get the cropped face region.
         * 
         * @param session Data container.
         * @param faceCrop Computed crop of the face.
         * @param maskCrop Mask used for the cropping. Will be computed in the method.
         * @param useAligned Switch for using the aligned image.
         * @param faceRegionAlpha Enlarge the face region by passing this parameter.
         */
        void GetCroppedImages(Session& session, cv::Mat& faceCrop, cv::Mat& maskCrop, bool useAligned, float faceRegionAlpha);
        
        /**
         * @brief Computation of the input features using different edge detectors.
         * 
         * @param image Input image.
         * @param mask  Input region of the face.
         * @param applyBlur Wheter or not rub a GaussianBlur before the edge detection.
         * @return cv::Mat Container storing the results of the different edge detectors.
         */
        cv::Mat GetClassifierFocusFeatures(cv::Mat& image, cv::Mat& mask, bool applyBlur);
    };
}