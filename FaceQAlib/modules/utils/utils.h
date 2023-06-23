#ifndef FACEQA_LIB_UTILS_H
#define FACEQA_LIB_UTILS_H

#include "faceqa_lib.h"


namespace cv
{
    class Mat;
}

struct Point2f
{
    float x;
    float y;
};



namespace FACEQA_LIB
{
    struct Point2i
    {
        int x;
        int y;
    };

    FACEQA_EXPORT void makeSquareBoundingBoxWithPadding(
        const FACEQA::BoundingBox& i_bb, 
        const cv::Mat& i_input_image,
        cv::Mat& o_output_image, 
        FACEQA::BoundingBox& o_bb,
        Point2i & o_translation_vector
        );

    FACEQA_EXPORT FACEQA::BoundingBox makeSquareBoundingBox(
        const FACEQA::BoundingBox& i_bb);

    FACEQA_EXPORT size_t findLargestBoundingBox(
        const std::vector<FACEQA::BoundingBox>& faceRects);

    // draw a face bounding box, just for debugging
    FACEQA_EXPORT void drawBoundingBoxes(
        const FACEQA::Image& faceImage,
        const std::vector<FACEQA::BoundingBox>& bBoxes,
        const std::string& destFilename);

    FACEQA_EXPORT void drawFaceLandmarks(
        const FACEQA::Image& faceImage,
        const FACEQA::FaceLandmarks& landmarks,
        const std::string& destFileName);

    FACEQA_EXPORT void drawFaceLandmarks(
        const FACEQA::Image& faceImage,
        const std::vector<FACEQA::FaceLandmarks>& multiLandmarks,
        const std::string& destFileName);

    FACEQA_EXPORT cv::Mat copyToCvImage(const FACEQA::Image& sourceImage, bool asGrayImage = false);
}

#endif