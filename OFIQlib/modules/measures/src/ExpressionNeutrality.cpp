/**
 * @file ExpressionNeutrality.cpp
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

#include "ExpressionNeutrality.h"
#include "FaceMeasures.h"
#include "OFIQError.h"
#include <fstream>
#include <opencv2/ml.hpp>
#include <cmath>

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::ExpressionNeutrality;
    static const std::string modelConfigItemCNN1 = "params.measures.ExpressionNeutrality.cnn1_model_path";
    static const std::string modelConfigItemCNN2 = "params.measures.ExpressionNeutrality.cnn2_model_path";
    static const std::string modelConfigItemAdaboost = "params.measures.ExpressionNeutrality.adaboost_model_path";

    static const uint16_t dimCNN1 = 224;
    static const uint16_t dimCNN2 = 260;

    ExpressionNeutrality::ExpressionNeutrality(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
        auto modelPathCNN1 = configuration.getDataDir() + "/" + configuration.GetString(modelConfigItemCNN1);
        auto modelPathCNN2 = configuration.getDataDir() + "/" + configuration.GetString(modelConfigItemCNN2);
        auto modelPathAdaboost = configuration.getDataDir() + "/" + configuration.GetString(modelConfigItemAdaboost);
        
        try
        {
            std::ifstream instream(modelPathCNN1, std::ios::in | std::ios::binary);

            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnvCNN1.initialize(modelData, dimCNN1, dimCNN1);
        }
        catch (std::exception e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::UnknownError,
                std::string("Loading CNN1 model for expression neutrality failed"));
        }

        try
        {
            std::ifstream instream(modelPathCNN2, std::ios::in | std::ios::binary);

            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnvCNN2.initialize(modelData, dimCNN2, dimCNN2);
        }
        catch (std::exception e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::UnknownError,
                std::string("Loading CNN2 model for expression neutrality failed"));
        }

        try
        {
            classifier = cv::ml::Boost::load(modelPathAdaboost);
        }
        catch (const std::exception&)
        {
            throw OFIQError(
                OFIQ::ReturnCode::UnknownError,
                std::string("Loading adaboost model for expression neutrality failed"));
        }

        SigmoidParameters defaultValues;
        defaultValues.h = 100.0;
        defaultValues.x0 = -5000.0;
        defaultValues.w = 5000.0;
        defaultValues.round = true;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void ExpressionNeutrality::Execute(OFIQ_LIB::Session& session)
    {
        cv::Mat aligned = session.getAlignedFace();
        auto cropped = aligned(cv::Rect(144, 148, 328, 340));

        auto transformed = cropped;
        cv::cvtColor(cropped, transformed, cv::COLOR_BGR2RGB);

        const cv::Scalar mean(0.485, 0.456, 0.406);
        const cv::Scalar std(0.229, 0.224, 0.225);

        transformed.convertTo(transformed, CV_32FC3);
        transformed /= 255.0;
        transformed -= mean;
        transformed /= std;
        cv::Mat resized1;
        cv::resize(transformed, resized1, cv::Size(dimCNN1, dimCNN1), 0, 0, cv::INTER_LINEAR);
        cv::Mat blob = cv::dnn::blobFromImage({ resized1 });

        std::vector<float> net_input;
        net_input.assign(blob.begin<float>(), blob.end<float>());
        auto outCNN1 = m_onnxRuntimeEnvCNN1.run(net_input);
        cv::Mat features1 = cv::Mat(1, 1280, CV_32F, outCNN1[0].GetTensorMutableData<float>());

        cv::Mat resized2;
        cv::resize(transformed, resized2, cv::Size(dimCNN2, dimCNN2), 0, 0, cv::INTER_LINEAR);
        blob = cv::dnn::blobFromImage({ resized2 });

        net_input.clear();
        net_input.assign(blob.begin<float>(), blob.end<float>());
        auto outCNN2 = m_onnxRuntimeEnvCNN2.run(net_input);
        cv::Mat features2 = cv::Mat(1, 1408, CV_32F, outCNN2[0].GetTensorMutableData<float>());

        cv::Mat features;
        cv::hconcat(features1, features2, features);
        
        cv::Mat predResults;
        this->classifier->predict(features, predResults, cv::ml::DTrees::PREDICT_SUM);
        double rawScore = predResults.at<float>(0, 0);
        SetQualityMeasure(session, qualityMeasure, rawScore, OFIQ::QualityMeasureReturnCode::Success);
    }
}