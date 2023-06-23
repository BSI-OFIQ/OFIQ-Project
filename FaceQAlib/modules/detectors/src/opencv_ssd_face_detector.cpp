#include "opencv_ssd_face_detector.h"
#include "FaceQaError.h"
#include "utils.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace FACEQA;
using namespace cv;
using namespace std;

namespace FACEQA_LIB::modules::detectors
{

    const std::string SSDFaceDetector::paramPrototxt = "faceDetection.prototxt_path";
    const std::string SSDFaceDetector::paramCaffemodel = "faceDetection.model_path";

    SSDFaceDetector::SSDFaceDetector(const Configuration& config)
    {
        config.GetBool(Configuration::Tags::generateDebugImages, saveDebugImages);
        const auto fileNameProtoTxt = config.GetString(paramPrototxt);
        const auto fileNameCaffeModel = config.GetString(paramCaffemodel);

        try
        {
            dnnNet =
                make_shared<dnn::Net>(dnn::readNetFromCaffe(fileNameProtoTxt, fileNameCaffeModel));
        }
        catch (const std::exception&)
        {
            throw FaceQaError(
                ReturnCode::FaceDetectionError,
                "failed to initialize opencv SDD face detector");
        }
    }

    std::vector<BoundingBox> SSDFaceDetector::UpdateFaces(Session& session)
    {
        if (!this->dnnNet)
            throw FaceQaError(
                ReturnCode::FaceDetectionError,
                "Opencv SDD face detector isn't initialized");

        auto& faceImage = session.image;

        const bool isRGB = faceImage.depth == 24;

        cv::Mat cvImage(
            faceImage.height,
            faceImage.width,
            isRGB ? CV_8UC3 : CV_8UC1,
            faceImage.data.get());

        // cv::imwrite("debug_cv_image_color.png", cvImage);
        if (!isRGB)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_GRAY2RGB);
        // else
        //     cv::cvtColor(cvImage, cvImage, cv::COLOR_RGB2BGR);

        // cv::imwrite("debug_cv_image_bgr.png", cvImage);

        cv::Scalar meanBGR = Scalar(104, 117, 123);
        bool doSwapRB = true, // need to swap RB for RGB images
            doCrop = false;

        // Create a 4D blob from the image.
        Mat blob = dnn::blobFromImage(cvImage, 1.0, Size(300, 300), meanBGR, doSwapRB, doCrop);

        // Run a model.
        dnnNet->setInput(blob /*, "", 1.0, mean*/);
        std::vector<Mat> netOuts;
        dnnNet->forward(netOuts);

        // Network produces output blob with a shape 1x1xNx7 where N is a number of
        // detections and an every detection is a vector of values
        // [batchId, classId, confidence, left, top, right, bottom]

        std::vector<BoundingBox> faceRects;

        std::vector<int> classIds;
        std::vector<float> confidences;
        //std::vector<Rect> boxes;
        static float confThreshold = 0.6f;
        static float minimalRelativeFaceSize = 0.05;// 1.0 / 8;
        for (size_t k = 0; k < netOuts.size(); k++)
        {
            float* data = reinterpret_cast<float*>(netOuts[k].data);
            for (size_t i = 0; i < netOuts[k].total(); i += 7)
            {
                float confidence = data[i + 2],
                    l = data[i + 3],
                    t = data[i + 4],
                    r = data[i + 5],
                    b = data[i + 6];

                // printf("face %d: conf = %.2f\n", n, confidence);
                if (confidence >= confThreshold && 
                    l > 0 &&
                    t > 0 &&
                    r < 1 &&
                    b < 1 &&
                    r - l > minimalRelativeFaceSize)
                {
                    int left = (int)round(l * cvImage.cols);
                    int top = (int)round(t * cvImage.rows);
                    int width = (int)round((r - l) * cvImage.cols);
                    int height = (int)round((b - t) * cvImage.rows);
                    
                    classIds.push_back((int)(data[i + 1]) - 1); // Skip 0th background class id.
                    //boxes.push_back(Rect(left, top, width, height));
                    confidences.push_back(confidence);

                    faceRects.push_back(BoundingBox(left, top, width, height, FaceDetectorType::OPENCVSSD));
                    // printf("face %d (%.2f): x,y,w,h = %d %d %d %d\n", n, confidence, left, top,
                    // width, height);
                }
            }
        }

        if (saveDebugImages)
            drawBoundingBoxes(session.image, faceRects, "debug_face_detections.jpg");

        return faceRects;
    }

}
