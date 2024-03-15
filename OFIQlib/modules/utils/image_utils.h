/**
 * @file image_utils.h
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
 * @brief Provides image utility functions such as color conversion, luminance computation etc.
 * @author OFIQ development team
 */
#pragma once

#include "ofiq_lib.h"
#include "Session.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

/**
 * @brief Provides implementations in OFIQ. 
 */
namespace OFIQ_LIB {
	
	using ExposureRange = std::array<int, 2>;
	
	/**
	 * @brief Converts a color as specified in ISO/IEC 29794-5.
	 * @param v An intensity value between 0 (black) and 1 (white).
	 * @return If \f$v>0.04045\f$, then \f$((v+0.055)/1.055)^{2.4}\f$ is returned; 
	 * otherwise, if \f$v\leq 0.04045\f$, then \f$v/12.92\f$ is returned.
	 */
	OFIQ_EXPORT double ColorConvert(double v);

	/**
	 * @brief Cubic flattening function.
	 * @param x Argument
	 * @param k Argument
	 * @param eps Argument \f$\epsilon\f$
	 * @return If \f$x\leq\epsilon\f$, then \f$(k\cdot x+16)/116\f$ is returned;
	 * otherwise, if \f$x>\epsilon\f$, then \f$\sqrt[3]{x}\f$ is returned.
	 */
	OFIQ_EXPORT double Cubic(double x, double k, double eps);

	/**
	 * @brief Computes CIELAB values \f$a^*\f$ and \f$b^*\f$ from a BGR image.
	 * @param[in] bgrImage BGR image
	 * @param[out] a CIELAB value \f$a^*\f$
	 * @param[out] b CIELAB value \f$b^*\f$
	 */
	OFIQ_EXPORT void ConvertBGRToCIELAB(const cv::Mat& bgrImage, double& a, double& b);

	/**
	 * @brief Converts a BGR image to the luminance image.
	 * @details The conversion is specified in the ISO/IEC 29794-5 standard
	 * and uses the function \link OFIQ_LIB::ColorConvert() ColorConvert() \endlink.
	 * @param bgrImage BGR image
	 * @return Luminance image.
	 */
	OFIQ_EXPORT cv::Mat GetLuminanceImageFromBGR(const cv::Mat& bgrImage );

	/**
	 * @brief Computes the left eye center, the right eye center, the (planar) inter-eye-distance
	 * and the eye to mouth distance from facial landmarks.
	 * @param[in] landmarks Facial landmarks
	 * @param[out] leftEyeCenter Left eye center computed from landmarks
	 * @param[out] rightEyeCenter Right eye center computed from landmarks
	 * @param[out] interEyeDistance Inter-eye distance computed from landmarks (does not consider the yaw angle).
	 * @param[out] eyeMouthDistance Distance from the eyes' midpoint to the mouth.
	 */
	OFIQ_EXPORT void CalculateReferencePoints(const OFIQ::FaceLandmarks& landmarks, 
		OFIQ::LandmarkPoint& leftEyeCenter, 
		OFIQ::LandmarkPoint& rightEyeCenter,
		double& interEyeDistance, 
		double& eyeMouthDistance);

	/**
	 * @brief Extracts regions being of interest for some measures (e.g. NaturalColour).
	 * @details Details can be found in the ISO/IEC 29794-5 standard for the Natural colour
	 * measure.
	 * @param[out] leftRegionOfInterest Rectangular region corresponding to the left eye
	 * @param[out] rightRegionOfInterest Rectangular region corresponding to the right eye
	 * @param[in] leftEyeCenter Center of the left eye
	 * @param[in] rightEyeCenter Center of the right eye
	 * @param[in] interEyeDistance Planar inter-eye distance
	 * @param[in] eyeMouthDistance Distance from the eyes' centers midpoint to the mouth
	 * @return Applies a heuristic to estimate two regions being of interest for
	 * the natural colour measure.
	 */
	OFIQ_EXPORT void CalculateRegionOfInterest(cv::Rect& leftRegionOfInterest,
		cv::Rect& rightRegionOfInterest, 
		const OFIQ::LandmarkPoint& leftEyeCenter, 
		const OFIQ::LandmarkPoint& rightEyeCenter,
		const double interEyeDistance, const double eyeMouthDistance);

	/**
	 * @brief Computes the normalized histogram from a luminance image in 256 chunks.
	 * @param[in] luminanceImage Luminance image as returned by \link OFIQ_LIB::GetLuminanceImageFromBGR()
	 * GetLuminanceImageFromBGR() \endlink.
	 * @param[in] maskImage The histogram is computed on pixels where the values of maskImage are non-zero.
	 * @param[out] histogram Array of length 256 where the histogram is stored.
	 */
	OFIQ_EXPORT void GetNormalizedHistogram(const cv::Mat& luminanceImage, const cv::Mat& maskImage, cv::Mat1f& histogram);

	/**
	 * @brief Helper function for some measures.
	 * @details The function is used by
	 * \link OFIQ_LIB::modules::measures::UnderExposurePrevention UnderExposurePrevention\endlink and
	 * \link OFIQ_LIB::modules::measures::OverExposurePrevention OverExposurePrevention\endlink class.
	 * Details can be found in the ISO/IEC 29794-5 standard.
     * @param session Session object containing the original facial image
	 * and pre-processing results
	 * computed by the \link OFIQ_LIB::OFIQImpl::performPreprocessing()
	 * OFIQImpl::performPreprocessing()\endlink method
	 * @param exposureRange Range of pixels for which the aspect is computed.
	 * @return Exposure computed from the inputs.
	 */
	OFIQ_EXPORT double CalculateExposure(const Session& session, const ExposureRange& exposureRange);

	/**
	 * @brief Helper function for some measures.
	 * @details The function is used by 
	 * \link OFIQ_LIB::modules::measures::UnderExposurePrevention UnderExposurePrevention\endlink and
	 * \link OFIQ_LIB::modules::measures::OverExposurePrevention OverExposurePrevention\endlink class.
	 * Details can be found in the ISO/IEC 29794-5 standard.
	 * @param luminanceImage luminance image.
	 * @param maskImage The mask on which the aspect is computed
	 * @param exposureRange Range of pixels for which the aspect is computed.
	 * @return Brightness aspect computed from the inputs.
	 */
	OFIQ_EXPORT double ComputeBrightnessAspect(
        const cv::Mat& luminanceImage, const cv::Mat& maskImage, const ExposureRange& exposureRange);
}
