/**
 * @file CompressionArtifacts.cpp
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

#include "CompressionArtifacts.h"
#include "OFIQError.h"
#include "FaceMeasures.h"
#include "FaceParts.h"

#include <fstream>

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::CompressionArtifacts;
    static const std::string cropConfigItem = "params.measures.CompressionArtifacts.crop";
    static const std::string dimConfigItem = "params.measures.CompressionArtifacts.dim";
    static const std::string modelConfigItem = "params.measures.CompressionArtifacts.model_path";

    CompressionArtifacts::CompressionArtifacts(
        const Configuration& configuration,
        Session& session)
        : Measure{ configuration, session, qualityMeasure }
    {
        SigmoidParameters defaultValues;
        defaultValues.h = 1.0;
        defaultValues.a = -0.0278;
        defaultValues.s = 103.0;
        defaultValues.x0 = 0.3308;
        defaultValues.w = 0.092;
        defaultValues.round = true;
        AddSigmoid(qualityMeasure, defaultValues);

        auto modelPath = configuration.getDataDir() + "/" + configuration.GetString(modelConfigItem);

        try
        {
            m_crop = (uint16_t)configuration.GetNumber(cropConfigItem);
        }
        catch (const std::exception& e)
        {
            m_crop = 184;
        }

        try
        {
            m_dim = (uint16_t)configuration.GetNumber(dimConfigItem);
        }
        catch (const std::exception& e)
        {
            m_dim = 248;
        }

        try
        {
            std::ifstream instream(modelPath, std::ios::in | std::ios::binary);

            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnv.initialize(modelData, m_dim, m_dim);
        }
        catch (std::exception e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::UnknownError,
                std::string("Loading model for compression artifacts failed"));
        }
    }

    void CompressionArtifacts::Execute(OFIQ_LIB::Session& session)
    {
        cv::Mat inputImage = session.getAlignedFace();
        auto width = inputImage.cols;
        auto height = inputImage.rows;

        auto cropped = inputImage(cv::Rect(m_crop, m_crop, width - 2 * m_crop, height - 2 * m_crop));

        auto transformed = cropped;
        cv::cvtColor(cropped, transformed, cv::COLOR_BGR2RGB);

        const cv::Scalar mean(123.7, 116.3, 103.5);
        const cv::Scalar std(58.4, 57.1, 57.4);
        
        transformed.convertTo(transformed, CV_32FC3);
        transformed -= mean;
        transformed /= std;
        cv::Mat blob = cv::dnn::blobFromImage({ transformed });

        std::vector<float> net_input;
        net_input.assign(blob.begin<float>(), blob.end<float>());
        auto out = m_onnxRuntimeEnv.run(net_input);
        auto outPtr = out[0].GetTensorMutableData<float>();

        auto rawScore = *outPtr;
        SetQualityMeasure(session, qualityMeasure, rawScore, OFIQ::QualityMeasureReturnCode::Success);
    }
}
