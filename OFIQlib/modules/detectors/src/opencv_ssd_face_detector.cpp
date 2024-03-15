/**
 * @file opencv_ssd_face_detector.cpp
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

#include "opencv_ssd_face_detector.h"
#include "OFIQError.h"
#include "utils.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace OFIQ;
using namespace cv;
using namespace std;

namespace OFIQ_LIB::modules::detectors
{

    SSDFaceDetector::SSDFaceDetector(const Configuration& config)
    {

        const std::string pathPrefix = "params.detector.ssd.";
        const std::string paramPrototxt = pathPrefix + "prototxt_path";
        const std::string paramCaffemodel = pathPrefix + "model_path";
        const std::string paramConfidenceThreshold = pathPrefix + "confidence_thr";
        const std::string paramPadding = pathPrefix + "padding";
        const std::string paramMinimalRelativeFaceSize = pathPrefix + "min_rel_face_size";


        confidenceThreshold = config.GetNumber(paramConfidenceThreshold);
        padding = config.GetNumber(paramPadding);
        minimalRelativeFaceSize = config.GetNumber(paramMinimalRelativeFaceSize);
        const auto fileNameProtoTxt = config.getDataDir() + "/" + config.GetString(paramPrototxt);
        const auto fileNameCaffeModel =
            config.getDataDir() + "/" + config.GetString(paramCaffemodel);

        try
        {
            dnnNet =
                make_shared<dnn::Net>(dnn::readNetFromCaffe(fileNameProtoTxt, fileNameCaffeModel));
        }
        catch (const std::exception&)
        {
            throw OFIQError(
                ReturnCode::FaceDetectionError,
                "failed to initialize opencv SDD face detector");
        }
    }

    std::vector<BoundingBox> SSDFaceDetector::UpdateFaces(Session& session)
    {
        if (!this->dnnNet)
            throw OFIQError(
                ReturnCode::FaceDetectionError,
                "Opencv SDD face detector isn't initialized");

        auto& faceImage = session.image();

        const bool isRGB = faceImage.depth == 24;

        cv::Mat cvImage(
            faceImage.height,
            faceImage.width,
            isRGB ? CV_8UC3 : CV_8UC1,
            faceImage.data.get());

        if (!isRGB)
            cv::cvtColor(cvImage, cvImage, cv::COLOR_GRAY2RGB);
        // else
        //     cv::cvtColor(cvImage, cvImage, cv::COLOR_RGB2BGR);

        int paddingHorizontal = 0;
        int paddingVertical = 0;
        if (padding > 0)
        {
            paddingHorizontal = faceImage.width * padding;
            paddingVertical = faceImage.height * padding;
            cv::copyMakeBorder(cvImage, cvImage, paddingVertical, paddingVertical, paddingHorizontal, paddingHorizontal, BORDER_CONSTANT);
        }

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
        for (size_t k = 0; k < netOuts.size(); k++)
        {
            float* data = (float*)netOuts[k].data;
            for (size_t i = 0; i < netOuts[k].total(); i += 7)
            {
                float confidence = data[i + 2], 
                    l = data[i + 3], 
                    t = data[i + 4], 
                    r = data[i + 5],
                    b = data[i + 6];

                // printf("face %d: conf = %.2f\n", n, confidence);
                if ((double)confidence >= confidenceThreshold &&
                    l > 0 &&
                    t > 0 &&
                    r < 1 &&
                    b < 1 &&
                    r - l > minimalRelativeFaceSize)
                {
                    int left = (int)round(l * cvImage.cols) - paddingHorizontal;
                    int top = (int)round(t * cvImage.rows) - paddingVertical;
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

        return faceRects;
    }

}
