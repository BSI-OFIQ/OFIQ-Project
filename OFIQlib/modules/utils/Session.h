/**
 * @file Session.h
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
 * @brief The session class is the data container used to distribute the image and additional data, 
 * including the data computed during the pre-processing.
 * @author OFIQ development team
 */
#pragma once

#include "ofiq_lib.h"
#include <opencv2/opencv.hpp>

/**
 * Namespace for OFIQ implementations. 
 */
namespace OFIQ_LIB
{
    /**
      * @brief Forward declaration.
      */
    struct NeuronalNetworkContainer;

    using EulerAngle = std::array<double, 3>;

    /**
     * @brief The session class is the data container used to distribute the image and additional data, 
 * including the data computed during the pre-processing.
     * @details One instance of this class contains the relevant face information used for the computation of the activated measures.
     * Most information is acquired during the pre-processing where the detection of the facial landmarks, the aligned image, etc. is computed.
     */
    class Session
    {
    public:

        /**
         * @brief Construct a new Session object
         * 
         * @param image Input image that shall be analysed.
         * @param assessment Container to staore the computed measures.
         */
        Session(const OFIQ::Image& image, OFIQ::FaceImageQualityAssessment& assessment)
            : _image{image},
              _assessment{assessment},
              id{GenerateId()}
        {
        }

        /**
         * @brief Acess reference to the input image, connected to this session.
         * @return input image reference.
         */
        const OFIQ::Image& image() const { return _image; }

        /**
         * @brief Access reference to the FaceImageQualityAssessment object, connected to this session.
         * @return quality assessment object reference.
         */
        OFIQ::FaceImageQualityAssessment& assessment() { return _assessment; }

        /**
         * @brief Access to the id connected to this session.
         * 
         * @return const std::string& Reference to the id of this session.
         */
        const std::string& Id() const { return id; }

        // use the session object as data container 

        /**
         * @brief Set the Detected Faces 
         * 
         * @param i_boundingBoxes Vector of face bounding boxes found by a face detector.
         */
        void setDetectedFaces(const std::vector<OFIQ::BoundingBox>& i_boundingBoxes);
        
        /**
         * @brief Get the Detected Faces 
         * 
         * @return std::vector<OFIQ::BoundingBox> Return the bounding boxes of faces found on the image.
         */
        std::vector<OFIQ::BoundingBox> getDetectedFaces() const;

#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC
        void setLandmarksAllFaces(const std::vector<OFIQ::FaceLandmarks>& i_detectedLandmarks);
        std::vector<OFIQ::FaceLandmarks> getLandmarksAllFaces() const;
#endif

        /**
         * @brief Set the Pose of the input image.
         * 
         * @param i_pose 
         */
        void setPose(const EulerAngle& i_pose);

        /**
         * @brief Get the Pose of the input image.
         * 
         * @return EulerAngle Pose of the ipnut image.
         */
        EulerAngle getPose() const;

        /**
         * @brief Set the Landmarks detected on the input image.
         * 
         * @param i_landmarks 
         */
        void setLandmarks(const OFIQ::FaceLandmarks& i_landmarks);
        
        /**
         * @brief Get the Landmarks detected on the input image.
         * 
         * @return OFIQ::FaceLandmarks 
         */
        OFIQ::FaceLandmarks getLandmarks() const;

        
        /**
         * @brief Set the Aligned Face Landmarks detected on the aligned image.
         * 
         * @param i_landmarks 
         */
        void setAlignedFaceLandmarks(const OFIQ::FaceLandmarks& i_landmarks);

        /**
         * @brief Get the Aligned Face Landmarks detected on the aligned image.
         * 
         * @return OFIQ::FaceLandmarks 
         */
        OFIQ::FaceLandmarks getAlignedFaceLandmarks() const;

        /**
         * @brief Set the Aligned Face Transformation Matrix
         * 
         * @param i_transformationMatrix 
         */
        void setAlignedFaceTransformationMatrix(const cv::Mat & i_transformationMatrix);


        /**
         * @brief Get the Aligned Face Transformation Matrix
         * 
         * @return cv::Mat 
         */
        cv::Mat getAlignedFaceTransformationMatrix() const;

        
        /**
         * @brief Set the Aligned Face 
         * 
         * @param i_alignedFace 
         */
        void setAlignedFace(const cv::Mat & i_alignedFace);
        
        /**
         * @brief Get the Aligned Face object
         * 
         * @return cv::Mat 
         */
        cv::Mat getAlignedFace() const;

        /**
         * @brief Set the Aligned Face Landmarked Region
         * 
         * @param i_alignedFaceRegion 
         */
        void setAlignedFaceLandmarkedRegion(const cv::Mat & i_alignedFaceRegion);
        
        /**
         * @brief Get the Aligned Face Landmarked Region
         * 
         * @return cv::Mat 
         */
        cv::Mat getAlignedFaceLandmarkedRegion() const;

        /**
         * @brief Set the Face Parsing Image, see \link OFIQ_LIB::modules::segmentations::FaceParsing \endlink).
         * 
         * @param i_parsingImage 
         */
        void setFaceParsingImage(const cv::Mat& i_parsingImage);
        
        /**
         * @brief Get the Face Parsing Image, see \link OFIQ_LIB::modules::segmentations::FaceParsing \endlink).
         * 
         * @return cv::Mat 
         */
        cv::Mat getFaceParsingImage() const;

        /**
         * @brief Set the Face Occlusion Segmentation Image, see \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation \endlink)
         * 
         * @param i_segmentationImage 
         */
        void setFaceOcclusionSegmentationImage(const cv::Mat& i_segmentationImage);

        /**
         * @brief Get the Face Occlusion Segmentation Image, see \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation \endlink) 
         * 
         * @return cv::Mat 
         */
        cv::Mat getFaceOcclusionSegmentationImage() const;

    private:
        /**
         * @brief Reference to the input image, connected to this session.
         * 
         */
        const OFIQ::Image& _image;

        /**
         * @brief Refernce to the FaceImageQualityAssessment object, connected to this session.
         * 
         */
        OFIQ::FaceImageQualityAssessment& _assessment;
        /**
         * @brief Container for the faces found on the input image.
         * 
         */
        std::vector<OFIQ::BoundingBox> detectedFaces;

#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC
        std::vector<OFIQ::FaceLandmarks> landmarksAllFaces;
#endif

        /**
         * @brief Container for storing the pose information.
         * 
         */
        EulerAngle pose;

        /**
         * @brief Container for storing the landmark information.
         * 
         */
        OFIQ::FaceLandmarks landmarks;

        /**
         * @brief Container for storing the landmark information of the aligned image.
         * 
         */
        OFIQ::FaceLandmarks alignedFaceLandmarks;

        /**
         * @brief Container for storing the transformation matrix that led to the aligned image.
         * 
         */
        cv::Mat alignedFaceTransformationMatrix;

        /**
         * @brief Container for storing the aligned image.
         * 
         */
        cv::Mat alignedFace;

        /**
         * @brief Container for storing the landmarks of the aligned face image
         * 
         */
        cv::Mat alignedFacelandmarkedRegion;

        /**
         * @brief Container for storing the segmented face image
         * 
         */
        cv::Mat faceParsingImage;

        /**
         * @brief Container for storing the result of the face occlusion segmented image.
         * 
         */
        cv::Mat faceOcclusionSegmentationImage;

        /**
         * @brief Method for generating uuid's for the session.
         * 
         * @return std::string 
         */
        std::string GenerateId() const;

        /**
         * @brief Container for storing the id of the session.
         * 
         */
        std::string id;
    };
}