/**
 * @file adnet_landmarks.cpp
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

#include "adnet_landmarks.h"
#include "OFIQError.h"
#include "utils.h"

#include <algorithm>
#include <fstream>
#include <onnxruntime_cxx_api.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace OFIQ_LIB::modules::landmarks
{
    using namespace OFIQ;
    using namespace std;

    class ADNetFaceLandmarkExtractorImpl
    {

    public:
        ADNetFaceLandmarkExtractorImpl() = default;

        ~ADNetFaceLandmarkExtractorImpl() = default;

        std::vector<float> extractLandMarks(cv::Mat& i_input_image)
        {
            // scale image
            cv::Mat scaled_image = scale_image_to_inputsize(i_input_image);
            // convert to input for the net
            std::vector<float> net_input = convert_to_net_input(scaled_image);
            if (net_input.size() != m_number_of_input_elements)
            {
                throw OFIQError(ReturnCode::FaceLandmarkExtractionError, "invalid image format.");
            }

            std::vector<float> landmarks_from_net = find_landmarks(net_input);

            return landmarks_from_net;
        }

        // init onnx session
        void init_session(const std::vector<uint8_t>& i_model_data)
        {
            m_ort_session = std::make_unique<Ort::Session>(
                m_ortenv,
                i_model_data.data(), 
                i_model_data.size(),
                Ort::SessionOptions{nullptr});


            get_parameter_from_model(
                m_expected_image_width,
                m_expected_image_height,
                m_expected_image_number_of_channels,
                m_number_of_input_elements);
        }

    private:
        std::vector<float> convert_to_net_input(cv::Mat& i_input_image)
        {
            // reshape to 1D
            auto image = i_input_image.reshape(1, 1);

            // Normalize and Convert to vector<float> from cv::Mat.
            std::vector<float> vec;
            image.convertTo(vec, CV_32FC1, 2. / 255, -1.);

            // Transpose (Height, Width, Channel)(224,224,3) to (Chanel, Height,
            // Width)(3,224,224)
            std::vector<float> output;
            for (size_t ch = 0; ch < 3; ++ch)
            {
                for (size_t i = ch; i < vec.size(); i += 3)
                {
                    output.emplace_back(vec[i]);
                }
            }

            return output;
        }

        cv::Mat scale_image_to_inputsize(cv::Mat& i_input_image)
        {

            if ((i_input_image.cols == m_expected_image_width) &&
                (i_input_image.rows == m_expected_image_height))
            {
                return i_input_image;
            }
            // resize
            // Create a new Mat object to store the scaled image
            cv::Mat scaled_image;

            // Perform the scaling operation
            cv::resize(
                i_input_image,
                scaled_image,
                cv::Size(m_expected_image_width, m_expected_image_height),
                0,
                0,
                cv::INTER_LINEAR);

            return scaled_image;
        }

        void get_parameter_from_model(
            int64_t& io_expected_image_width,
            int64_t& io_expected_image_height,
            int64_t& io_expected_image_number_of_channels,
            int64_t& io_number_of_input_elements)
        {
            auto type_info = m_ort_session->GetInputTypeInfo(0);
            auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
            auto input_node_shape = tensor_info.GetShape();

            io_expected_image_number_of_channels = input_node_shape[1];
            io_expected_image_width = input_node_shape[2];
            io_expected_image_height = input_node_shape[3];
            io_number_of_input_elements = io_expected_image_number_of_channels *
                                          io_expected_image_width * io_expected_image_height;
        }

        std::vector<float> find_landmarks(std::vector<float>& i_image)
        {

            // define shape
            const std::array<int64_t, 4> inputShape = {
                1,
                m_expected_image_number_of_channels,
                m_expected_image_height,
                m_expected_image_width};


            // define Tensor
            auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
            auto inputTensor = Ort::Value::CreateTensor<float>(
                memory_info,
                i_image.data(),
                i_image.size(),
                inputShape.data(),
                inputShape.size());


            // define names
            Ort::AllocatorWithDefaultOptions ort_alloc;
            Ort::AllocatedStringPtr inputName = m_ort_session->GetInputNameAllocated(0, ort_alloc);
            const std::array<const char*, 1> inputNames = {inputName.get()};
            std::vector<const char*> outputNames;
            const size_t num_output_nodes = m_ort_session->GetOutputCount();
            for (int i = 0; i < num_output_nodes; i++)
            {
                Ort::AllocatedStringPtr outputName =
                    m_ort_session->GetOutputNameAllocated(i, ort_alloc);
                outputNames.push_back(outputName.get());
                outputName.release();
            }

            inputName.release();


            // run inference
            try
            {
                Ort::RunOptions runOptions;
                auto results = m_ort_session->Run(
                    runOptions,
                    inputNames.data(),
                    &inputTensor,
                    1,
                    outputNames.data(),
                    num_output_nodes);
                size_t useThisOutput =
                    num_output_nodes - 1; // take last output like in python implementation

                auto element = results[useThisOutput].GetTensorTypeAndShapeInfo();
                std::vector<int64_t> shape = element.GetShape();



                auto elementPtr = results[useThisOutput].GetTensorMutableData<float>();

                std::vector<float> landmarks(elementPtr, elementPtr + element.GetElementCount());

                // undo normalization
                std::transform(
                    landmarks.cbegin(),
                    landmarks.cend(),
                    landmarks.begin(),
                    [](float i_landmark) { return (i_landmark + 1.) / 2 * 255; });

                return landmarks;
            }
            catch (Ort::Exception& e)
            {
                std::stringstream errmsg;
                errmsg << "Ort::Exception: " << e.what();
                throw OFIQError(ReturnCode::FaceLandmarkExtractionError, errmsg.str());
            }

            return std::vector<float>();
        }


    private:
        Ort::Env m_ortenv;
        std::unique_ptr<Ort::Session> m_ort_session;

        int64_t m_expected_image_width = 0;
        int64_t m_expected_image_height = 0;
        int64_t m_expected_image_number_of_channels = 0;
        int64_t m_number_of_input_elements = 0;
    };

    //--------------------------------------------------
    // End of class ADNetFaceLandmarkExtractorImpl
    //--------------------------------------------------

    ADNetFaceLandmarkExtractor::ADNetFaceLandmarkExtractor(
        const Configuration& config)
    {
        try
        {

            landmarkExtractor = std::make_unique<ADNetFaceLandmarkExtractorImpl>();
            const auto modelPath =
                config.getDataDir() + "/" + config.GetString("params.landmarks.ADNet.model_path");

            std::ifstream instream(modelPath, std::ios::in | std::ios::binary);
            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());

            landmarkExtractor->init_session(modelData);
        }
        catch (const std::exception&)
        {
            throw OFIQError(
                ReturnCode::FaceLandmarkExtractionError,
                "failed to initialize ADNet face landmark extractor");
        }
    }

    ADNetFaceLandmarkExtractor::~ADNetFaceLandmarkExtractor() = default;

    OFIQ::FaceLandmarks ADNetFaceLandmarkExtractor::updateLandmarks(Session& session)
    {
        OFIQ::FaceLandmarks landmarks;
        std::vector<OFIQ::BoundingBox> faceRects;
        try
        {
            faceRects = session.getDetectedFaces();
        }
        catch (const std::exception& e)
        {
            std::string err_msg = "no face found on given image: " + std::string(e.what());
            throw OFIQError(ReturnCode::FaceDetectionError, err_msg);
        }
        
        if (faceRects.empty())
        {
            return landmarks;
        }

        const size_t faceIndex = 0; // take largest face found
        OFIQ::BoundingBox detectedFace = faceRects[faceIndex];

        cv::Mat cvImage = copyToCvImage(session.image());
        Point2i translationVector{ 0, 0 };

        if (detectedFace.faceDetector == FaceDetectorType::OPENCVSSD) {
            // SSD bounding box does not have to be quadratic -> check and make it square
            cv::Mat cvImage_maybe_padded;
            OFIQ::BoundingBox detectedFaceSquare;
            
            OFIQ_LIB::makeSquareBoundingBoxWithPadding(
                detectedFace,
                cvImage,
                cvImage_maybe_padded,
                detectedFaceSquare,
                translationVector
            );
            cvImage = cvImage_maybe_padded;
            detectedFace = detectedFaceSquare;

        } // if opencvssd

        // crop image
        // Define the region of interest (ROI) for cropping
        cv::Rect roi(
            detectedFace.xleft,
            detectedFace.ytop,
            detectedFace.width,
            detectedFace.height); // (x, y, width, height)

        // Crop the image using the ROI
        cv::Mat croppedImage = cvImage(roi);

        std::vector<float> landmarks_from_net = landmarkExtractor->extractLandMarks(croppedImage);
        float scalingFactor = detectedFace.height / 256.0f;

        int offset_x = detectedFace.xleft - translationVector.x;
        int offset_y = detectedFace.ytop - translationVector.y;
        for (int i = 0; i < landmarks_from_net.size(); i += 2)
        {
            auto x = static_cast<int>(
                std::round(landmarks_from_net[i] * scalingFactor));
            auto y = static_cast<int>(
                std::round(landmarks_from_net[i+1] * scalingFactor));
            x += offset_x;
            y += offset_y;
            landmarks.landmarks.emplace_back(
                LandmarkPoint(static_cast<uint16_t>(x), static_cast<uint16_t>(y)));
        }

        landmarks.type = LandmarkType::LM_98;

        return landmarks;
    }

#ifdef OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC
#pragma message ("WARNING: This is needed for SingleFacePresent measure based on T-metric which is slow. This should be removed after adjustment of 29794-5 standard.")
// TODO: If 29794-5 is adujusted as suggested, then remove all code
//     within OFIQ_SINGLE_FACE_PRESENT_WITH_TMETRIC and keep 
//     alternative variant within #else block.
    
    // protected override
    std::vector<OFIQ::FaceLandmarks> ADNetFaceLandmarkExtractor::updateLandmarksAllFaces
    (OFIQ_LIB::Session& session, const std::vector<OFIQ::BoundingBox>& faces)
    {
        std::vector<OFIQ::FaceLandmarks> landmarksList;

        for (auto& face : faces)
        {
            OFIQ::BoundingBox detectedFace = face;
            OFIQ::FaceLandmarks landmarks;

            cv::Mat cvImage = copyToCvImage(session.image());
            Point2i translationVector{ 0, 0 };

            if (detectedFace.faceDetector == FaceDetectorType::OPENCVSSD) {
                // SSD bounding box does not have to be quadratic -> check and make it square
                cv::Mat cvImage_maybe_padded;
                OFIQ::BoundingBox detectedFaceSquare;

                OFIQ_LIB::makeSquareBoundingBoxWithPadding(
                    detectedFace,
                    cvImage,
                    cvImage_maybe_padded,
                    detectedFaceSquare,
                    translationVector
                );
                cvImage = cvImage_maybe_padded;
                detectedFace = detectedFaceSquare;

            } // if opencvssd

            // crop image
            // Define the region of interest (ROI) for cropping
            cv::Rect roi(
                detectedFace.xleft,
                detectedFace.ytop,
                detectedFace.width,
                detectedFace.height); // (x, y, width, height)

            // Crop the image using the ROI
            cv::Mat croppedImage = cvImage(roi);

            std::vector<float> landmarks_from_net = landmarkExtractor->extractLandMarks(croppedImage);
            float scalingFactor = detectedFace.height / 256.0f;

            int offset_x = detectedFace.xleft - translationVector.x;
            int offset_y = detectedFace.ytop - translationVector.y;
            for (int i = 0; i < landmarks_from_net.size(); i += 2)
            {
                auto x = static_cast<int>(
                    std::floor(landmarks_from_net[i] * scalingFactor));
                auto y = static_cast<int>(
                    std::floor(landmarks_from_net[i + 1] * scalingFactor));
                x += offset_x;
                y += offset_y;
                landmarks.landmarks.emplace_back(
                    LandmarkPoint(static_cast<uint16_t>(x), static_cast<uint16_t>(y)));
            }

            landmarks.type = LandmarkType::LM_98;
            landmarksList.push_back(landmarks);
        }
    
        return landmarksList;
    }
#endif
}