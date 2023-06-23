#include "utils.h"
#include "FaceParts.h"
#include "FaceQaError.h"
#include "PartExtractor.h"

#include <algorithm>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

// #include <opencv2/highgui.hpp>


using PartExtractor = FACEQA_LIB::modules::landmarks::PartExtractor;
using FaceParts = FACEQA_LIB::modules::landmarks::FaceParts;

namespace FACEQA_LIB
{


    FACEQA_EXPORT void makeSquareBoundingBoxWithPadding(
        const FACEQA::BoundingBox& i_bb,
        const cv::Mat& i_input_image,
        cv::Mat& o_output_image,
        FACEQA::BoundingBox& o_bb,
        Point2i & o_translation_vector)
    {
        int height_image = i_input_image.rows;
        int width_image = i_input_image.cols;

        o_translation_vector.x = 0;
        o_translation_vector.y = 0;

        o_bb = FACEQA_LIB::makeSquareBoundingBox(i_bb);


        int x_top_left = o_bb.xleft;
        int y_top_left = o_bb.ytop;
        int x_bottom_right = x_top_left + o_bb.width;
        int y_bottom_right = y_top_left + o_bb.height;

        if ((x_top_left < 0) || (x_bottom_right > width_image) || (y_top_left < 0) ||
            (y_bottom_right > height_image))
        {
            // Define the border sizes
            int topBorder = std::max(0, -y_top_left);
            int bottomBorder = std::max(0, y_bottom_right - height_image);
            int leftBorder = std::max(0, -x_top_left);
            int rightBorder = std::max(0, x_bottom_right - width_image);


            o_translation_vector.x = leftBorder;
            o_translation_vector.y = topBorder;

            // adapt coordinates
            o_bb.xleft += o_translation_vector.x;
            o_bb.ytop += o_translation_vector.y;

            cv::copyMakeBorder(
                i_input_image,
                o_output_image,
                topBorder,
                bottomBorder,
                leftBorder,
                rightBorder,
                cv::BORDER_CONSTANT,
                cv::Scalar(0, 0, 0));
        }
        else
        {
            o_output_image = i_input_image.clone();
        }
    }

    FACEQA_EXPORT FACEQA::BoundingBox makeSquareBoundingBox(const FACEQA::BoundingBox& i_bb)
    {

        // if already square
        if (i_bb.height == i_bb.width)
        {
            return i_bb;
        }

        int16_t x_top_left_orig = i_bb.xleft;
        int16_t y_top_left_orig = i_bb.ytop;

        int16_t x_bottom_right_orig = i_bb.xleft + i_bb.width;
        int16_t y_bottom_right_orig = i_bb.ytop + i_bb.height;

        int16_t x_top_left_trans, y_top_left_trans, x_bottom_right_trans, y_bottom_right_trans = 0;

        // extend bounding box to square
        if (i_bb.width < i_bb.height)
        {
            // extend width

            x_top_left_trans =
                std::floor((x_bottom_right_orig + x_top_left_orig - i_bb.height) * 0.5);
            x_bottom_right_trans =
                std::ceil((x_bottom_right_orig + x_top_left_orig + i_bb.height) * 0.5);

            y_top_left_trans = y_top_left_orig;
            y_bottom_right_trans = y_bottom_right_orig;
        }
        else
        {
            // extend height
            x_top_left_trans = x_top_left_orig;
            x_bottom_right_trans = x_bottom_right_orig;

            y_top_left_trans =
                std::floor((y_bottom_right_orig + y_top_left_orig - i_bb.width) * 0.5);
            y_bottom_right_trans =
                std::ceil((y_bottom_right_orig + y_top_left_orig + i_bb.width) * 0.5);
        }

        FACEQA::BoundingBox square(
            x_top_left_trans // int16_t xleft
            ,
            y_top_left_trans // int16_t ytop
            ,
            x_bottom_right_trans - x_top_left_trans // int16_t width
            ,
            y_bottom_right_trans - y_top_left_trans // int16_t height
            ,
            i_bb.faceDetector // FaceDetectorType i_faceDetector
        );

        return square;
    }

    FACEQA_EXPORT size_t findLargestBoundingBox(const std::vector<FACEQA::BoundingBox>& faceRects)
    {
        if (faceRects.empty())
            throw FaceQaError(
                FACEQA::ReturnCode::UnknownError,
                "Unable to find largest face in an empty list");

        // find largest bounding box (an iterator pointing to it)
        std::vector<FACEQA::BoundingBox>::const_iterator idxBiggestFace = std::max_element(
            faceRects.cbegin(),
            faceRects.cend(),
            [](const FACEQA::BoundingBox& a, const FACEQA::BoundingBox& b)
            { return a.width * a.height < b.width * b.height; });

        return (size_t)std::distance(faceRects.begin(), idxBiggestFace);
    }

    FACEQA_EXPORT void drawBoundingBoxes(
        const FACEQA::Image& faceImage,
        const std::vector<FACEQA::BoundingBox>& bBoxes,
        const std::string& destFilename)
    {
        cv::Mat cvImage = copyToCvImage(faceImage);

        for (auto& bBox : bBoxes)
            cv::rectangle(
                cvImage,
                cv::Rect(bBox.xleft, bBox.ytop, bBox.width, bBox.height),
                cv::Scalar(0, 0, 255),
                2);

        cv::imwrite(destFilename, cvImage);
    }

    FACEQA_EXPORT void drawFaceLandmarks(
        const FACEQA::Image& faceImage,
        const FACEQA::FaceLandmarks& landmarks,
        const std::string& destFileName)
    {
        std::vector<FACEQA::FaceLandmarks> multiLandmarks{landmarks};
        return drawFaceLandmarks(faceImage, multiLandmarks, destFileName);
    }

    FACEQA_EXPORT void drawFaceLandmarks(
        const FACEQA::Image& faceImage,
        const std::vector<FACEQA::FaceLandmarks>& multiLandmarks,
        const std::string& destFileName)
    {
        cv::Mat cvImage = copyToCvImage(faceImage);

        for (auto& landmarks : multiLandmarks)
            for (auto& landmark : landmarks.landmarks)
                cv::drawMarker(
                    cvImage,
                    cv::Point(landmark.x, landmark.y),
                    cv::Scalar(0, 0, 255),
                    cv::MARKER_CROSS,
                    3);

        cv::imwrite(destFileName, cvImage);
    }

    FACEQA_EXPORT cv::Mat copyToCvImage(const FACEQA::Image& sourceImage, bool asGrayImage)
    {
        bool isRGB = sourceImage.depth == 24;

        cv::Mat cvImage(sourceImage.height, sourceImage.width, isRGB ? CV_8UC3 : CV_8UC1);

        memcpy(cvImage.data, sourceImage.data.get(), sourceImage.size());

        if (!isRGB && !asGrayImage)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_GRAY2BGR);
        else if (isRGB && !asGrayImage)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_RGB2BGR);
        else if (isRGB && asGrayImage)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_RGB2GRAY);
        else if (!isRGB && asGrayImage)
        {
            // Then no work is necessary
        }

        return cvImage;
    }
}