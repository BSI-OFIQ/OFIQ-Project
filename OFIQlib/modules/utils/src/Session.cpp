/**
 * @file Session.cpp
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

#include "Session.h"

namespace OFIQ_LIB
{
    
    std::string Session::GenerateId() const
    {
        static int sessionCounter = 0;
        sessionCounter += 1;
        return std::to_string(sessionCounter);
    }

    void Session::setDetectedFaces(const std::vector<OFIQ::BoundingBox>& i_boundingBoxes) {
        detectedFaces = i_boundingBoxes;        
    }

    std::vector<OFIQ::BoundingBox> Session::getDetectedFaces()  const
    {
        return detectedFaces;
    }

#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC
    void Session::setLandmarksAllFaces(const std::vector<OFIQ::FaceLandmarks>& i_detectedLandmarks)
    {
        landmarksAllFaces = i_detectedLandmarks;
    }

    std::vector<OFIQ::FaceLandmarks> Session::getLandmarksAllFaces() const
    {
        return landmarksAllFaces;
    }
#endif

    void Session::setPose(const EulerAngle& i_pose) {
        pose = i_pose;
    }

    EulerAngle Session::getPose() const
    {
        return pose;
    }

    void Session::setLandmarks(const OFIQ::FaceLandmarks& i_landmarks) {
        landmarks = i_landmarks;
    }

    OFIQ::FaceLandmarks Session::getLandmarks() const
    {
        return landmarks;
    }

    void Session::setAlignedFaceLandmarks(const OFIQ::FaceLandmarks& i_landmarks) {
        alignedFaceLandmarks = i_landmarks;
    }

    OFIQ::FaceLandmarks Session::getAlignedFaceLandmarks() const
    {
        return alignedFaceLandmarks;
    }

    void Session::setAlignedFaceTransformationMatrix(const cv::Mat& i_transformationMatrix) {
        alignedFaceTransformationMatrix = i_transformationMatrix.clone();
    }

    cv::Mat Session::getAlignedFaceTransformationMatrix() const
    {
        return alignedFaceTransformationMatrix;
    }

    void Session::setAlignedFace(const cv::Mat& i_alignedFace) {
        alignedFace = i_alignedFace.clone();
    }

    cv::Mat Session::getAlignedFace() const
    {
        return alignedFace.clone();
    }

    void Session::setAlignedFaceLandmarkedRegion(const cv::Mat& i_alignedFaceRegion) {
        alignedFacelandmarkedRegion = i_alignedFaceRegion.clone();
    }

    cv::Mat Session::getAlignedFaceLandmarkedRegion() const
    {
        return alignedFacelandmarkedRegion.clone();
    }

    void Session::setFaceParsingImage(const cv::Mat& i_parsingImage)
    {
        faceParsingImage = i_parsingImage.clone();
    }

    cv::Mat Session::getFaceParsingImage() const
    {
        return faceParsingImage.clone();
    }

    void Session::setFaceOcclusionSegmentationImage(const cv::Mat& i_segmentationImage)
    {
        faceOcclusionSegmentationImage = i_segmentationImage.clone();
    }

    cv::Mat Session::getFaceOcclusionSegmentationImage() const
    {
        return faceOcclusionSegmentationImage.clone();
    }

}