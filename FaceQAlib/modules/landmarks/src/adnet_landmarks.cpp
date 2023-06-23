#include <algorithm>
#include <fstream>

#include <onnxruntime_cxx_api.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "FaceQaError.h"
#include "utils.h"

#include "adnet_landmarks.h"


//#define DEBUGSHOWIMAGES 1

namespace FACEQA_LIB::modules::landmarks
{
    using namespace FACEQA;
    using namespace std;

    class ADNetFaceLandmarkExtractorImpl
    {

    public:
        ADNetFaceLandmarkExtractorImpl() = default;

        FACEQA::FaceLandmarks extractLandMarks(cv::Mat& i_input_image)
        {
            // scale image
            cv::Mat scaled_image = scale_image_to_inputsize(i_input_image);

            // compute scaling factor
            float scale_factor_x = (float)scaled_image.cols / i_input_image.cols;
            float scale_factor_y = (float)scaled_image.rows / i_input_image.rows;

            // Display the original and cropped images
#ifdef DEBUGSHOWIMAGES
            cv::imshow("Scaled Image", scaled_image);
            cv::waitKey(0);
#endif
            // convert to input for the net
            std::vector<float> net_input = convert_to_net_input(scaled_image);
            if (net_input.size() != m_number_of_input_elements)
            {
                throw FaceQaError(ReturnCode::FaceLandmarkExtractionError, "invalid image format.");
            }


            std::vector<float> landmarks_from_net = find_landmarks(net_input);

            // undo scaling on landmark coordinates
            for (int i = 0; i < landmarks_from_net.size(); i++) {
                float scaling_factor = ( i % 2 == 0 ) ? scale_factor_x : scale_factor_y;
                landmarks_from_net[i] = landmarks_from_net[i] / scaling_factor;
            }

#ifdef DEBUGSHOWIMAGES

            // Define the points to be printed
            std::vector<cv::Point> points;
            //for (auto iter = landmarks_from_net.begin(); iter != landmarks_from_net.end(); ++iter)
            for (int i = 0; i < landmarks_from_net.size(); i++)
            {
                int x = landmarks_from_net[i++];
                int y = landmarks_from_net[i];
                points.push_back(cv::Point(x, y));
            }


            // Print the points on the image
            for (const auto& point : points)
            {
                cv::circle(
                    i_input_image,
                    point,
                    3,
                    cv::Scalar(0, 0, 255),
                    -1); // Draw a red filled circle of radius 5
            }

            // Display the image with the points
            cv::imshow("Scaled image with landmarks", i_input_image);
            cv::waitKey(0);
#endif


            // convert
            FACEQA::FaceLandmarks l_landmarks;

            for (int i = 0; i < landmarks_from_net.size(); i++)
            {
                int x = landmarks_from_net[i++];
                int y = landmarks_from_net[i];
                l_landmarks.landmarks.emplace_back(
                    std::move(LandmarkPoint(x, y)));
            }

            l_landmarks.type = LandmarkType::ADNet;
            return l_landmarks;
        }

        // init onnx session
        void init_session(const void* i_model_data, size_t i_model_data_length)
        {
            m_ort_session = std::make_unique<Ort::Session>(
                m_ortenv,
                i_model_data,
                i_model_data_length,
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
                throw FaceQaError(ReturnCode::FaceLandmarkExtractionError, errmsg.str());
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
        const Configuration& config, FaceDetectorInterface& detector)
        : detector{detector}
    {

        // TODO:
        // currently (june 2023) limitted to use ssd facefinder

        config.GetBool(Configuration::Tags::generateDebugImages, saveDebugImages);
        // const auto filenameLandmarksModel = config.GetString(paramLandmarksModel);

        try
        {

            landmarkExtractor = std::make_unique<ADNetFaceLandmarkExtractorImpl>();
            const auto modelPath = config.GetString("faceLandmarks.model_path");

            std::ifstream instream(modelPath, std::ios::in | std::ios::binary);
            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());

            landmarkExtractor->init_session(modelData.data(), modelData.size());
        }
        catch (const std::exception&)
        {
            throw FaceQaError(
                ReturnCode::FaceLandmarkExtractionError,
                "failed to initialize ADNet face landmark extractor");
        }
    }

    ADNetFaceLandmarkExtractor::~ADNetFaceLandmarkExtractor() = default;

    FACEQA::FaceLandmarks ADNetFaceLandmarkExtractor::updateLandmarks(Session& session)
    {
        FACEQA::FaceLandmarks landmarks;
        std::vector<FACEQA::BoundingBox> faceRects;
        try
        {
            faceRects = detector.detectFaces(session);
        }
        catch (const std::exception& e)
        {
            std::string err_msg = "no face found on given image: " + std::string(e.what());
            throw FaceQaError(
                ReturnCode::FaceDetectionError, err_msg
                );
        }
        

        if (faceRects.empty())
        {
            // Todo: logging?
            return landmarks;
        }

        const size_t faceIndex = detector.biggestFaceId();
        FACEQA::BoundingBox detectedFace = faceRects[faceIndex];

        cv::Mat cvImage = copyToCvImage(session.image);
        int height_image = session.image.height;
        int width_image = session.image.width;

        Point2i translationVector;

        if (detectedFace.faceDetector == FaceDetectorType::OPENCVSSD) {
            // SSD bounding box does not have to be quadratic -> check and make it square
            cv::Mat cvImage_maybe_padded;
            FACEQA::BoundingBox detectedFaceSquare;
            
            FACEQA_LIB::makeSquareBoundingBoxWithPadding(
                detectedFace ,          // const FACEQA::BoundingBox& i_bb,
                cvImage,                // const cv::Mat& i_input_image,
                cvImage_maybe_padded,   // cv::Mat& o_output_image,
                detectedFaceSquare,     // FACEQA::BoundingBox& o_bb,
                translationVector       // cv::Point& o_translation_vector
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


#if DEBUGSHOWIMAGES
        cv::imshow("Original Image", cvImage);
        cv::imshow("cropped Image", croppedImage);
        cv::waitKey(0);
#endif

        landmarks = landmarkExtractor->extractLandMarks(croppedImage);

        // transform landmarks to full image coordinates
        for (auto & lm : landmarks.landmarks) {
            lm.x += detectedFace.xleft - translationVector.x;
            // check borders
            if (lm.x < 0)
                lm.x = 0;
            if (lm.x >= width_image)
                lm.x = width_image - 1;

            lm.y += detectedFace.ytop - translationVector.y;
            if (lm.y < 0)
                lm.y = 0;
            if (lm.y >= height_image)
                lm.y = height_image -1;

#ifdef DEBUGSHOWIMAGES
            std::cout << "x: " << lm.x << "    y: " << lm.y << std::endl;
#endif
        }
        
        if (saveDebugImages)
        {
            drawFaceLandmarks(session.image, landmarks, "debug_face_landmarks.jpg");
        }

        return landmarks;
    }


}