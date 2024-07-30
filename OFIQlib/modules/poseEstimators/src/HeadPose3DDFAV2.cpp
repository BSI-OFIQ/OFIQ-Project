/**
 * @file HeadPose3DDFAV2.cpp
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

#define _USE_MATH_DEFINES
#include <cmath>
#include "HeadPose3DDFAV2.h"
#include "OFIQError.h"
#include "FaceMeasures.h"
#include "AllPoseEstimators.h"
#include "utils.h"
#include <fstream>

namespace OFIQ_LIB::modules::poseEstimators
{
    const std::string HeadPose3DDFAV2::m_paramPoseEstimatorModel = "params.measures.HeadPose.model_path";
    const cv::Mat paramMean = (cv::Mat_<float>(1, 7) <<
        3.4926363e-04, 2.5279013e-07, -6.8751979e-07, 6.0167957e+01,
        -6.2955132e-07, 5.7572004e-04, -5.0853912e-05);
    const cv::Mat paramStd = (cv::Mat_<float>(1, 7) <<
        1.76321526e-04, 6.73794348e-05, 4.47084894e-04, 2.65502319e+01,
        1.23137695e-04, 4.49302170e-05, 7.92367064e-05);

    HeadPose3DDFAV2::HeadPose3DDFAV2(const Configuration& config)
    {
        const auto modelPath =
            config.getDataDir() + "/" + config.GetString(m_paramPoseEstimatorModel);
        try
        {
            std::ifstream instream(modelPath, std::ios::in | std::ios::binary);
            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());

            m_ortSession = std::make_unique<Ort::Session>(m_ortenv, modelData.data(), modelData.size(), Ort::SessionOptions{ nullptr });

            auto type_info = m_ortSession->GetInputTypeInfo(0);
            auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
            auto input_node_shape = tensor_info.GetShape();

            m_expectedImageNumberOfChannels = input_node_shape[1];
            m_expectedImageWidth = input_node_shape[2];
            m_expectedImageHeight = input_node_shape[3];
            m_numberOfInputElements = m_expectedImageNumberOfChannels * m_expectedImageWidth * m_expectedImageHeight;
            // define shape
            m_inputShape = { 1, m_expectedImageNumberOfChannels, m_expectedImageHeight, m_expectedImageWidth };
        }
        catch (const std::exception&)
        {
            throw OFIQError(
                OFIQ::ReturnCode::UnknownError,
                std::string("Loading HeadPose 3DDFAV2 model failed"));
        }
    }

    void HeadPose3DDFAV2::updatePose(OFIQ_LIB::Session& session, EulerAngle& pose)
    {
        const auto cvImageBGR = copyToCvImage(session.image());
        auto biggestFace = session.getDetectedFaces()[0];

        cv::Mat croppedImageBGR = CropImage(cvImageBGR, biggestFace);

        cv::Mat resizedImage;
        cv::resize(croppedImageBGR, resizedImage, cv::Size(m_expectedImageWidth, m_expectedImageHeight), 0, 0, cv::INTER_LINEAR);
        resizedImage.convertTo(resizedImage, CV_32FC3);
        cv::Mat normalizedImageBGR;
        normalizedImageBGR = resizedImage - cv::Scalar(127.5, 127.5, 127.5);
        normalizedImageBGR /= cv::Scalar(128.0, 128.0, 128.0);

        // hwc -> chw
        auto channelSize = normalizedImageBGR.rows * normalizedImageBGR.cols;
        std::vector<float> tensor(3 * channelSize);
        for (int i = 0; i < channelSize; i++)
        {
            const cv::Point2i point(i % normalizedImageBGR.cols, i / normalizedImageBGR.cols);
            const auto& pixel = normalizedImageBGR.at<cv::Vec3f>(point);
            for (int j = 0; j < 3; j++)
                tensor[i + j * channelSize] = pixel[j];
        }

        // define Tensor
        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
        auto inputTensor = Ort::Value::CreateTensor<float>(
            memory_info,
            &tensor[0],
            m_numberOfInputElements,
            m_inputShape.data(),
            m_inputShape.size());
        const std::array<const char*, 1> inputNames = { "input" };
        const std::array<const char*, 1> outputNames = { "output" };

        // run inference
        std::vector<Ort::Value> results;
        try
        {
            Ort::RunOptions runOptions;
            results = m_ortSession->Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), 1);
        }
        catch (Ort::Exception& e)
        {
            std::stringstream errmsg;
            errmsg << "3DDFAV2 model Ort::Exception: " << e.what();
            throw OFIQError(OFIQ::ReturnCode::UnknownError, errmsg.str());
        }
        auto element = results[0].GetTensorTypeAndShapeInfo();
        auto elementPtr = results[0].GetTensorMutableData<float>();
        std::vector<float> output(elementPtr, elementPtr + element.GetElementCount());

        cv::Mat paramOutput(1, 7, CV_32FC1, output.data());
        cv::Mat param = paramOutput.mul(paramStd) + paramMean;
        cv::Mat r0 = (cv::Mat_<float>(1, 3) << param.at<float>(0), param.at<float>(1), param.at<float>(2));
        cv::Mat r1 = (cv::Mat_<float>(1, 3) << param.at<float>(4), param.at<float>(5), param.at<float>(6));
        r0 /= cv::norm(r0);
        r1 /= cv::norm(r1);
        cv::Mat r2 = r0.cross(r1);
        std::vector<cv::Mat> matrices = { r0, r1, r2 };
        cv::Mat rotationMatrix;
        cv::vconcat(matrices, rotationMatrix);
        cv::transpose(rotationMatrix, rotationMatrix);
        rotationMatrix.convertTo(rotationMatrix, CV_64FC1);

        std::vector<double> angles(3);
        const static double thres = 0.9975;
        const double r11 = rotationMatrix.at<double>(0, 0);
        const double r12 = rotationMatrix.at<double>(0, 1);
        const double r13 = rotationMatrix.at<double>(0, 2);
        const double r21 = rotationMatrix.at<double>(1, 0);
        const double r22 = rotationMatrix.at<double>(1, 1);
        const double r23 = rotationMatrix.at<double>(1, 2);
        const double r31 = rotationMatrix.at<double>(2, 0);
        const double r32 = rotationMatrix.at<double>(2, 1);
        const double r33 = rotationMatrix.at<double>(2, 2);

        double phi_pitch = 0.0;
        double phi_yaw = 0.0;
        double phi_roll = 0.0;
        if (r31 <= thres && r31 >= -thres)
        {
            phi_pitch = asin(r31);
            double s = 1.0 / cos(phi_pitch);
            phi_yaw = -atan2(s*r32, s*r33);
            phi_roll = -atan2(s*r21, s*r11);
        }
        else if (r31 < -thres )
        {
            phi_pitch = -0.5 * M_PI;
            phi_yaw = -atan2(r12, r13);
            phi_roll = 0.0;
        }
        else /* if ( r31 > thres */
        {
            phi_pitch = 0.5 * M_PI;
            phi_yaw = atan2(r12,r13);
            phi_roll = 0.0;
        }

        phi_pitch *= 180.0 / M_PI;
        phi_yaw *= 180.0 / M_PI;
        phi_roll *= 180.0 / M_PI;

        angles[0] = phi_yaw;
        angles[1] = phi_pitch;
        angles[2] = phi_roll;

        pose[0] = angles[0]; // Yaw
        pose[1] = angles[1]; // Pitch
        pose[2] = angles[2]; // Roll
    }

    cv::Mat HeadPose3DDFAV2::CropImage(const cv::Mat& image, const OFIQ::BoundingBox& detectedFace)
    {
        double centerX = detectedFace.xleft + detectedFace.width / 2.0;
        double centerY = detectedFace.ytop + detectedFace.height / 2.0;
        auto b = (int16_t)(centerY - 0.44 * detectedFace.height);
        auto d = (int16_t)(centerY + 0.51 * detectedFace.height);
        auto a = (int16_t)(centerX - (d - b) / 2.0);
        int16_t c = a + (d - b);
        OFIQ::BoundingBox box;
        box.xleft = a;
        box.ytop = b;
        box.width = c - a;
        box.height = d - b;
        cv::Mat paddedImage;
        OFIQ::BoundingBox croppedBox;
        Point2i translationVector;
        OFIQ_LIB::makeSquareBoundingBoxWithPadding(box, image, paddedImage, croppedBox, translationVector);

        // crop image
        // Define the region of interest (ROI) for cropping
        cv::Rect roi(
            croppedBox.xleft,
            croppedBox.ytop,
            croppedBox.width,
            croppedBox.height); // (x, y, width, height)

        // Crop the image using the ROI
        cv::Mat croppedImage = paddedImage(roi);
        return croppedImage;
    }

}