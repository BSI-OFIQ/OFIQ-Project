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


        m_confidenceThreshold = config.GetNumber(paramConfidenceThreshold);
        m_padding = config.GetNumber(paramPadding);
        m_minimalRelativeFaceSize = config.GetNumber(paramMinimalRelativeFaceSize);
        const auto fileNameProtoTxt = config.getDataDir() + "/" + config.GetString(paramPrototxt);
        const auto fileNameCaffeModel =
            config.getDataDir() + "/" + config.GetString(paramCaffemodel);

        try
        {
            m_dnnNet =
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
        if (!this->m_dnnNet)
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

        int paddingHorizontal = 0;
        int paddingVertical = 0;
        if (m_padding > 0)
        {
            paddingHorizontal = static_cast<int>(faceImage.width * m_padding);
            paddingVertical = static_cast<int>(faceImage.height * m_padding);
            cv::Mat paddedImage{
                cvImage.rows + paddingVertical * 2,
                cvImage.cols + paddingHorizontal * 2,
                cvImage.type() };
            cv::copyMakeBorder(cvImage, paddedImage, paddingVertical, paddingVertical, paddingHorizontal, paddingHorizontal, BORDER_CONSTANT);
            cvImage = paddedImage;
        }

        auto meanBGR = Scalar(104, 117, 123);
        bool doSwapRB = true; // need to swap RB for RGB images
        bool doCrop = false;

        // Create a 4D blob from the image.
        Mat blob = dnn::blobFromImage(cvImage, 1.0, Size(300, 300), meanBGR, doSwapRB, doCrop);

        // Run a model.
        m_dnnNet->setInput(blob /*, "", 1.0, mean*/);
        std::vector<Mat> netOuts;
        m_dnnNet->forward(netOuts);

        // Network produces output blob with a shape 1x1xNx7 where N is a number of
        // detections and an every detection is a vector of values
        // [batchId, classId, confidence, left, top, right, bottom]

        std::vector<BoundingBox> faceRects;

        std::vector<int> classIds;
        std::vector<float> confidences;
        for (auto output : netOuts)
        {
            const float* data = (float*)output.data;
            for (size_t i = 0; i < output.total(); i += 7)
            {
                float confidence = data[i + 2];
                float l = data[i + 3]; 
                float t = data[i + 4]; 
                float r = data[i + 5];
                float b = data[i + 6];

                if ((double)confidence >= m_confidenceThreshold &&
                    l > 0 &&
                    t > 0 &&
                    r < 1 &&
                    b < 1 &&
                    r - l > m_minimalRelativeFaceSize)
                {
                    auto left = static_cast<int>(round(l * static_cast<float>(cvImage.cols))) - paddingHorizontal;
                    auto top = static_cast<int>(round(t * static_cast<float>(cvImage.rows))) - paddingVertical;
                    auto width = static_cast<int>(round((r - l) * static_cast<float>(cvImage.cols)));
                    auto height = static_cast<int>(round((b - t) * static_cast<float>(cvImage.rows)));
                    
                    classIds.push_back((int)(data[i + 1]) - 1); // Skip 0th background class id.
                    confidences.push_back(confidence);

                    faceRects.push_back
                        (BoundingBox(static_cast<int16_t>(left), static_cast<int16_t>(top), 
                                     static_cast<int16_t>(width), static_cast<int16_t>(height),
                                     FaceDetectorType::OPENCVSSD));
                }
            }
        }

        return faceRects;
    }

}
