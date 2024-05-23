/**
 * @file OFIQImpl.cpp
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

#include "Configuration.h"
#include "Executor.h"
#include "ofiq_lib_impl.h"
#include "OFIQError.h"
#include "FaceMeasures.h"
#include "utils.h"
#include "image_io.h"

using namespace std;
using namespace OFIQ;
using namespace OFIQ_LIB;
using namespace OFIQ_LIB::modules::measures;


OFIQImpl::OFIQImpl():m_emptySession({this->dummyImage, this->dummyAssement}) {}

ReturnStatus OFIQImpl::initialize(const std::string& configDir, const std::string& configFilename)
{
    try
    {
        this->config = std::make_unique<Configuration>(configDir, configFilename);
        CreateNetworks();
        m_executorPtr = CreateExecutor(m_emptySession);
    }
    catch (const OFIQError & ex)
    {
        return {ex.whatCode(), ex.what()};
    }
    catch (const std::exception & ex)
    {
        return {ReturnCode::UnknownError, ex.what()};
    }

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus OFIQImpl::scalarQuality(const OFIQ::Image& face, double& quality)
{
    FaceImageQualityAssessment assessments;

    if (auto result = vectorQuality(face, assessments); 
        result.code != ReturnCode::Success)
        return result;

    if(assessments.qAssessments.find(QualityMeasure::UnifiedQualityScore) != assessments.qAssessments.end())
        quality = assessments.qAssessments[QualityMeasure::UnifiedQualityScore].scalar;
    else
    {
        // just as an example - the scalarQuality is an average of all valid scalar measurements
        double sumScalars = 0;
        int numScalars = 0;
        for (auto const& [measureName, aResult] : assessments.qAssessments)
        {
            if (aResult.scalar != -1)
            {
                sumScalars += aResult.scalar;
                numScalars++;
            }
        }
        quality = numScalars != 0 ? sumScalars / numScalars : 0;
    }

    return ReturnStatus(ReturnCode::Success);
}

/**
 * method opens up a window displaying the given image. window has given title.
*/
void previewWindow(std::string title, cv::Mat& image)
{
    cv::namedWindow(title, cv::WINDOW_NORMAL);
    cv::imshow(title, image);
    cv::waitKey(0);
}


/**
 * method draws the given bounding box onto the image. the bounding box will have the given color. 
*/
void drawBoundingBox(cv::Mat& image, OFIQ::BoundingBox& bb, cv::Scalar& color)
{
    //Corner points of bounding box
    cv::Point2f top_left = cv::Point2f(bb.xleft, bb.ytop);
    cv::Point2f top_right = cv::Point2f(bb.xleft + bb.width, bb.ytop);

    cv::Point2f bottom_left = cv::Point2f(bb.xleft, bb.ytop + bb.height);
    cv::Point2f bottom_right = cv::Point2f(bb.xleft + bb.width, bb.ytop + bb.height);

    //draw top line
    cv::line(image, top_left, top_right, color, cv::LINE_AA);

    //draw left line
    cv::line(image, top_left, bottom_left, color, cv::LINE_AA);

    //draw bottom line
    cv::line(image, bottom_left, bottom_right, color, cv::LINE_AA);

    //draw right line
    cv::line(image, top_right, bottom_right, color, cv::LINE_AA);
}

void drawLandmarkPoint(cv::Mat& image, OFIQ::LandmarkPoint& fp, cv::Scalar& color, int index)
{
    //create center point (just for readability)
    cv::Point2i center = cv::Point2i(fp.x, fp.y);
    cv::Point2i text_origin = cv::Point2i(fp.x + 15, fp.y + 15);
    //draw landmark point 
    cv::circle(image, center, 9, color, cv::FILLED);
    cv::putText(image, std::to_string(index) , text_origin, cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0,0,0), 3);
}

/**
 * method draws all found bounding boxes onto the image.
*/
void visualizeBoundingBoxes(Session& session, const std::vector<OFIQ::BoundingBox>& boxes)
{
    cv::Mat image = copyToCvImage(session.image());
    
    //NOTE: color values are in BGR!
    cv::Scalar purple = cv::Scalar(191, 0, 207);

    //draw all bounding boxes onto the image
    for(BoundingBox bb : boxes)
    {
        drawBoundingBox(image, bb, purple);
    }

    //open window
    previewWindow("Bounding Boxes Preview", image);
}

void visualizeLandmarks(Session& session, const std::vector<OFIQ::FaceLandmarks>& landmarks)
{
    cv::Mat image = copyToCvImage(session.image());

    //NOTE: color values are in BGR!
    cv::Scalar orange = cv::Scalar(224, 110, 0);
    
    //go through all detected facelandmarks for each face on the image
    int index = 0;
    for(FaceLandmarks fl : landmarks)
    {   
        //go through eacht specific landmark point and draw it onto the image
        for(LandmarkPoint fp : fl.landmarks)
        {
            drawLandmarkPoint(image, fp, orange, index);
            index++;
        }
    }
    //open window
    previewWindow("Landmark Preview", image);
}

void OFIQImpl::performPreprocessing(Session& session)
{
    log("\t1. detectFaces ");
    //find Bounding Boxes
    std::vector<OFIQ::BoundingBox> faces = networks->faceDetector->detectFaces(session);

    if (faces.empty())
    {
        log("\n\tNo faces were detected, abort preprocessing\n");
        throw OFIQError(ReturnCode::FaceDetectionError, "No faces were detected");
    }
    session.setDetectedFaces(faces);

    visualizeBoundingBoxes(session, faces);

    log("2. estimatePose ");
    session.setPose(networks->poseEstimator->estimatePose(session));

    log("3. extractLandmarks ");
#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC
    session.setLandmarksAllFaces(networks->landmarkExtractor->extractLandmarksAllFaces(session, session.getDetectedFaces()));
    if (!session.getLandmarksAllFaces().empty())
    {
        session.setLandmarks(session.getLandmarksAllFaces().front());
    }
    else
    {
        session.setLandmarks(networks->landmarkExtractor->extractLandmarks(session));
    }
#else
    session.setLandmarks(networks->landmarkExtractor->extractLandmarks(session));
#endif
    

    visualizeLandmarks(session, session.getLandmarksAllFaces());

    log("4. alignFaceImage ");
    // aligned face requires the landmarks of the face thus it must come after the landmark extraction.
    alignFaceImage(session);

    log("5. getSegmentationMask ");
    // segmentation results for face_parsing
    session.setFaceParsingImage(OFIQ_LIB::copyToCvImage(
        networks->segmentationExtractor->GetMask(
            session,
            OFIQ_LIB::modules::segmentations::SegmentClassLabels::face),
        true));

    log("6. getFaceOcclusionMask ");
    session.setFaceOcclusionSegmentationImage(OFIQ_LIB::copyToCvImage(
        networks->faceOcclusionExtractor->GetMask(
            session,
            OFIQ_LIB::modules::segmentations::SegmentClassLabels::face),
        true));

    static const std::string alphaParamPath = "params.measures.FaceRegion.alpha";
    double alpha = 0.0f;
    try
    {
        alpha = this->config->GetNumber(alphaParamPath);
    }
    catch(...)
    {
        alpha = 0.0f;
    }

    log("7. getAlignedFaceMask ");

    session.setAlignedFaceLandmarkedRegion(
         OFIQ_LIB::modules::landmarks::FaceMeasures::GetFaceMask(
            session.getAlignedFaceLandmarks(),
            session.getAlignedFace().rows,
            session.getAlignedFace().cols,
            (float)alpha
         )
    );
    log("\npreprocessing finished\n");
}

void OFIQImpl::alignFaceImage(Session& session) {
    auto landmarks = session.getLandmarks();
    OFIQ::FaceLandmarks alignedFaceLandmarks;
    alignedFaceLandmarks.type = landmarks.type;
    cv::Mat transformationMatrix;
    cv::Mat aligned = alignImage(session.image(), landmarks, alignedFaceLandmarks, transformationMatrix);

    session.setAlignedFace(aligned);
    session.setAlignedFaceLandmarks(alignedFaceLandmarks);
    session.setAlignedFaceTransformationMatrix(transformationMatrix);

}

ReturnStatus OFIQImpl::vectorQuality(
    const OFIQ::Image& image, OFIQ::FaceImageQualityAssessment& assessments)
{
    auto session = Session(image, assessments);

    try
    {
        log("perform preprocessing:\n");
        performPreprocessing(session);
    }
    catch (const OFIQError& e)
    {
        log("OFIQError: " + std::string(e.what()) + "\n");
        for (const auto& measure : m_executorPtr->GetMeasures() )
        {
            auto qualityMeasure = measure->GetQualityMeasure();
            switch (qualityMeasure)
            {
            case QualityMeasure::Luminance:
                session.assessment().qAssessments[QualityMeasure::LuminanceMean] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                session.assessment().qAssessments[QualityMeasure::LuminanceVariance] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                break;
            case QualityMeasure::CropOfTheFaceImage:
                session.assessment().qAssessments[QualityMeasure::LeftwardCropOfTheFaceImage] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                session.assessment().qAssessments[QualityMeasure::RightwardCropOfTheFaceImage] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                session.assessment().qAssessments[QualityMeasure::UpwardCropOfTheFaceImage] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                session.assessment().qAssessments[QualityMeasure::DownwardCropOfTheFaceImage] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                break;
            case QualityMeasure::HeadPose:
                session.assessment().qAssessments[QualityMeasure::HeadPoseYaw] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                session.assessment().qAssessments[QualityMeasure::HeadPosePitch] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                session.assessment().qAssessments[QualityMeasure::HeadPoseRoll] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                break;
            default:
                session.assessment().qAssessments[measure->GetQualityMeasure()] =
                { 0, -1, OFIQ::QualityMeasureReturnCode::FailureToAssess };
                break;
            }
        }

        return { e.whatCode(), e.what() };
    }

    log("execute assessments:\n");
    m_executorPtr->ExecuteAll(session);

    return ReturnStatus(ReturnCode::Success);
}

OFIQ_EXPORT std::shared_ptr<Interface> Interface::getImplementation()
{
    return std::make_shared<OFIQImpl>();
}
