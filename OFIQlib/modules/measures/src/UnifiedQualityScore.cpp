/**
 * @file UnifiedQualityScore.cpp
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

#include "UnifiedQualityScore.h"
#include "utils.h"
#include "OFIQError.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>

namespace OFIQ_LIB::modules::measures
{
    static const auto qualityMeasure = OFIQ::QualityMeasure::UnifiedQualityScore;
    static const std::string paramModelpath = "params.measures.UnifiedQualityScore.model_path";
    static const int imageSize = 112;
    static const int cropLeft = 40;
    static const int cropRight = 40;
    static const int cropTop = 33;
    static const int cropBottom = 47;
    static const int scaledWidth = 192;
    static const int scaledHeight = 192;

    UnifiedQualityScore::UnifiedQualityScore(const Configuration& configuration)
        : Measure{ configuration, qualityMeasure }
    {
        try
        {
            SigmoidParameters defaultValues;
            defaultValues.h = 100.0;
            defaultValues.x0 = 23.0;
            defaultValues.w = 2.6;
            defaultValues.round = true;
            AddSigmoid(qualityMeasure, defaultValues);

            std::string modelPath = configuration.getDataDir()+"/"+configuration.GetString(paramModelpath);

            std::ifstream instream(modelPath, std::ios::in | std::ios::binary);

            std::vector<uint8_t> modelData(
                (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>());
            m_onnxRuntimeEnv.initialize(modelData, imageSize,imageSize); 
        }
        catch (std::exception& e)
        {
            throw OFIQError(
                OFIQ::ReturnCode::UnknownError,
                std::string("Loading magface model failed"));
        }
    }

    static cv::Mat CreateBlob(const cv::Mat& image)
    {
        cv::Mat converted;
        image.convertTo(converted, CV_32FC3);
        converted /= 255.0;
        cv::Size size(imageSize, imageSize);
        bool swapRB = false;
        return cv::dnn::blobFromImage({ converted }, 1.0, size, 0, swapRB);
    }

    void UnifiedQualityScore::Execute(OFIQ_LIB::Session & session)
    {
        cv::Mat alignedFaceBGR = session.getAlignedFace();

        cv::resize(alignedFaceBGR, alignedFaceBGR, cv::Size(scaledWidth, scaledHeight));
        cv::Mat alignedFaceCropBGR = alignedFaceBGR(
            cv::Range(cropTop, scaledHeight - cropBottom),
            cv::Range(cropLeft, scaledWidth - cropRight));
        auto blob = CreateBlob(alignedFaceCropBGR);
        
        std::vector<float> net_input;
        net_input.assign(blob.begin<float>(), blob.end<float>());
        auto out = m_onnxRuntimeEnv.run(net_input);
        auto outPtr = out[0].GetTensorMutableData<float>();
        double rawScore = outPtr[0];
        SetQualityMeasure(session, qualityMeasure, rawScore, OFIQ::QualityMeasureReturnCode::Success);
    }
}