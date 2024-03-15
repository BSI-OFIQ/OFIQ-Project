/**
 * @file utils.h
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
 * @brief Helper functions used by several classes.
 * @author OFIQ development team
 */
#ifndef OFIQ_LIB_UTILS_H
#define OFIQ_LIB_UTILS_H

#include "ofiq_lib.h"

/**
 * @brief OpenCV's namespace.
 */
namespace cv
{
    /**
     * @brief Forward declaration of the OpenCV class cv::Mat.
     */
    class Mat;
}

/**
 * @brief Representation of a point with floating point arithmetics.
 * 
 */
struct Point2f
{
    float x;
    float y;
};



namespace OFIQ_LIB
{
    /**
     * @brief Representation of a point with integer arithmetics.
     * 
     */
    struct Point2i
    {
        int x;
        int y;
    };

    /**
     * @brief Some computations, especially neural networks, need a squarred image as input.
     * This funtion consumes a boundig box and an input image. 
     * The greater parameter of width or height is used to define the side length of the new squarred bounding box.
     * The face will be centered in the bounding box. Padding is added if needed.
     * The squarred bounding box is used generate a new cropped image, the o_output_image.
     * Required translations are described by the translation vector o_translation_vector.
     * 
     * @param i_bb Initial bounding box.
     * @param i_input_image  Input image.
     * @param o_output_image Cropped output image. Cropping is based on the computed squarred bounding box.
     * @param o_bb Squarred bounding box.
     * @param o_translation_vector Translation vector.
     */
    OFIQ_EXPORT void makeSquareBoundingBoxWithPadding(
        const OFIQ::BoundingBox& i_bb, 
        const cv::Mat& i_input_image,
        cv::Mat& o_output_image, 
        OFIQ::BoundingBox& o_bb,
        Point2i & o_translation_vector
        );

    /**
     * @brief This function converts a non-squarred bounding box into an squarred one. The side length is defined by the greater one of height or width.
     * 
     * @param i_bb Input bounding box.
     * @return OFIQ::BoundingBox Squarred bounding box.
     */
    OFIQ_EXPORT OFIQ::BoundingBox makeSquareBoundingBox(
        const OFIQ::BoundingBox& i_bb);

    /**
     * @brief This function returns the position of the largest bounding box (largest in terms of area) from a vector of bounding boxes.
     * 
     * @param faceRects Vector containing bounding boxes.
     * @return size_t Position of the largest bounding box in the vector.
     */
    OFIQ_EXPORT size_t findLargestBoundingBox(
        const std::vector<OFIQ::BoundingBox>& faceRects);

    /**
     * @brief Convert images in OFIQ::Image format into the OpenCV cv::Mat format. The image can be converted from color to gray scale by setting the parameter asGrayImage to true.
     * 
     * @param sourceImage Input image.
     * @param asGrayImage Switch for adding gray scale conversion.
     * @return cv::Mat Input image in cv::Mat format.
     */
    OFIQ_EXPORT cv::Mat copyToCvImage(const OFIQ::Image& sourceImage, bool asGrayImage = false);

    /**
     * @brief This function transforms a face image so that the position of the eyes, nose and mouth are roughly at a pre-defined position. Face alignment is the translation, rotation and scaling of the image to do this.
     * 
     * @param faceImage Input image.
     * @param faceLandmarks  Face landmarks, based on the face represented in the input image.
     * @param alignedFaceLandmarks  Face landmarks of the aligned face image.
     * @param transformationMatrix Transformation matrix used to transform the landmarks.
     * @return cv::Mat Aligned face image with a resolution of 616x616.
     */
    OFIQ_EXPORT cv::Mat alignImage(
        const OFIQ::Image& faceImage,
        const OFIQ::FaceLandmarks& faceLandmarks,
        OFIQ::FaceLandmarks& alignedFaceLandmarks,
        cv::Mat& transformationMatrix);

    /**
     * @brief Based on face landmarks the center of the left and right eye are computed.
     * 
     * @param faceLandmarks Input face landmarks.
     * @param leftEyeCenter  Point coordinates of the left eye center.
     * @param rightEyeCenter Point coordinates of the right eye center.
     */
    OFIQ_EXPORT void calculateEyeCenter(
        const OFIQ::FaceLandmarks& faceLandmarks,
        Point2f& leftEyeCenter,
        Point2f& rightEyeCenter);

    /**
     * @brief This function generates a gray scaled image with the resolution passed by the call.
     * 
     * @param width Width of the generated image.
     * @param height Height of the generated image.
     * @return OFIQ::Image Generated gray scaled image.
     */
    OFIQ_EXPORT OFIQ::Image MakeGreyImage(uint16_t width, uint16_t height);

    /**
     * @brief Based on the provided landmarks this function computes the distance between the point between the eyes and the chin.
     * 
     * @param faceLandmarks Input face landmarks.
     * @return float Computed distance.
     */
    OFIQ_EXPORT float tmetric(const OFIQ::FaceLandmarks& faceLandmarks);

    /**
     * @brief Based on a given rotation matrix this functions computes and returns the corresponding Euler angles.
     * 
     * @param R Input rotation matrix
     * @param angles Container with the computed Euler angles for [x,y,z] axis
     */
    OFIQ_EXPORT void rotationMatrixToEulerAngles(const cv::Mat& R, std::vector<double>& angles);
}

#endif