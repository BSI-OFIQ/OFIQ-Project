/**
 * @file InMemoryConfiguration.cpp
 *
 * @copyright Copyright (c) 2026  Federal Office for Information Security, Germany
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

#include "InMemoryConfiguration.h"

#include "OFIQError.h"

#include <filesystem>
#include <magic_enum.hpp>

namespace OFIQ_LIB
{
    namespace
    {
        const std::string PrefixMeasures = "params.measures.";
        const std::string PrefixDetector = "params.detector.ssd.";
        const std::string PrefixLandmarks = "params.landmarks.ADNet.";
        const std::string SigmoidSuffix = ".Sigmoid.";
        const std::string FaceOcclusionModel =
            "face_occlusion_segmentation/face_occlusion_segmentation_ort.onnx";
        const std::string FaceParsingModel = "face_parsing/bisenet_400.onnx";

        bool measureExists(const OFIQ::Configuration& config, OFIQ::QualityMeasure measure)
        {
            return config.measures.find(measure) != config.measures.end();
        }
    }

    InMemoryConfiguration::InMemoryConfiguration(const OFIQ::Configuration& config)
        : m_config(config)
    {
    }

    std::string InMemoryConfiguration::getDataDir() const
    {
        if (m_hasDataDirOverride)
            return m_dataDirOverride;
        return m_config.modelsBasePath;
    }

    void InMemoryConfiguration::SetDataDir(std::string_view dataDir)
    {
        m_dataDirOverride = dataDir;
        m_hasDataDirOverride = true;
    }

    bool InMemoryConfiguration::GetStringList(
        const std::string& key,
        std::vector<std::string>& value) const
    {
        if (key != "measures")
            return false;

        value.clear();
        for (const auto& [measure, measureConfig] : m_config.measures)
        {
            if (measure == OFIQ::QualityMeasure::NotSet || !measureConfig.enabled)
                continue;
            value.push_back(std::string(magic_enum::enum_name(measure)));
        }
        return true;
    }

    bool InMemoryConfiguration::GetBool(const std::string& key, bool& value) const
    {
        if (key == "params.measures.Sharpness.use_aligned_landmarks")
        {
            const auto it = m_config.measures.find(OFIQ::QualityMeasure::Sharpness);
            if (it == m_config.measures.end())
                return false;

            const auto param = it->second.params.find("use_aligned_landmarks");
            value = param != it->second.params.end() && param->second != 0.0;
            return true;
        }

        if (key.size() > PrefixMeasures.size() + SigmoidSuffix.size() + 5 &&
            key.compare(key.size() - 5, 5, "round") == 0)
        {
            const size_t sigmoidStart = key.find(SigmoidSuffix, PrefixMeasures.size());
            if (sigmoidStart == std::string::npos)
                return false;

            const std::string measureName =
                key.substr(PrefixMeasures.size(), sigmoidStart - PrefixMeasures.size());
            const auto measure = magic_enum::enum_cast<OFIQ::QualityMeasure>(measureName);
            if (!measure.has_value() || !measureExists(m_config, measure.value()))
                return false;

            value = m_config.measures.find(measure.value())->second.sigmoid.roundToInteger;
            return true;
        }

        return false;
    }

    bool InMemoryConfiguration::GetString(const std::string& key, std::string& value) const
    {
        if (key == PrefixDetector + "model_path")
        {
            value = m_config.detector.modelPath;
            return true;
        }
        if (key == PrefixDetector + "prototxt_path")
        {
            value = m_config.detector.prototxtPath;
            return true;
        }
        if (key == PrefixLandmarks + "model_path")
        {
            value = m_config.landmarks.modelPath;
            return true;
        }
        if (key == "params.measures.ExpressionNeutrality.cnn1_model_path" ||
            key == "params.measures.ExpressionNeutrality.cnn2_model_path" ||
            key == "params.measures.ExpressionNeutrality.adaboost_model_path")
        {
            const auto it = m_config.measures.find(OFIQ::QualityMeasure::ExpressionNeutrality);
            if (it == m_config.measures.end())
                return false;

            const auto paramName = key.substr(key.find_last_of('.') + 1);
            const auto pathIt = it->second.additionalModelPaths.find(paramName);
            if (pathIt == it->second.additionalModelPaths.end())
                return false;
            value = pathIt->second;
            return true;
        }
        if (key == "params.measures.FaceOcclusionSegmentation.model_path")
        {
            value = FaceOcclusionModel;
            return true;
        }
        if (key == "params.measures.FaceParsing.model_path")
        {
            value = FaceParsingModel;
            return true;
        }

        if (key.rfind(PrefixMeasures, 0) == 0 && key.size() > PrefixMeasures.size() + 11 &&
            key.compare(key.size() - 11, 11, ".model_path") == 0)
        {
            const std::string measureName = key.substr(
                PrefixMeasures.size(),
                key.size() - PrefixMeasures.size() - 11);
            const auto measure = magic_enum::enum_cast<OFIQ::QualityMeasure>(measureName);
            if (!measure.has_value())
                return false;

            const auto it = m_config.measures.find(measure.value());
            if (it == m_config.measures.end())
                return false;

            value = it->second.modelPath;
            return true;
        }

        return false;
    }

    bool InMemoryConfiguration::GetNumber(const std::string& key, double& value) const
    {
        if (key == PrefixDetector + "confidence_thr")
        {
            value = m_config.detector.confidenceThreshold;
            return true;
        }
        if (key == PrefixDetector + "padding")
        {
            value = m_config.detector.padding;
            return true;
        }
        if (key == PrefixDetector + "min_rel_face_size")
        {
            value = m_config.detector.minRelativeFaceSize;
            return true;
        }
        if (key == "params.measures.FaceRegion.alpha")
        {
            value = 0.0;
            return true;
        }
        if (key == "params.measures.Sharpness.face_region_alpha")
        {
            const auto it = m_config.measures.find(OFIQ::QualityMeasure::Sharpness);
            if (it == m_config.measures.end())
            {
                value = 0.0;
                return true;
            }

            const auto param = it->second.params.find("face_region_alpha");
            value = param == it->second.params.end() ? 0.0 : param->second;
            return true;
        }

        if (key.rfind(PrefixMeasures, 0) == 0)
        {
            const size_t sigmoidStart = key.find(SigmoidSuffix, PrefixMeasures.size());
            if (sigmoidStart != std::string::npos)
            {
                const std::string measureName =
                    key.substr(PrefixMeasures.size(), sigmoidStart - PrefixMeasures.size());
                const auto measure = magic_enum::enum_cast<OFIQ::QualityMeasure>(measureName);
                if (!measure.has_value())
                    return false;

                const auto it = m_config.measures.find(measure.value());
                if (it == m_config.measures.end())
                    return false;

                const std::string param = key.substr(sigmoidStart + SigmoidSuffix.size());
                const auto& sigmoid = it->second.sigmoid;
                if (param == "h")
                    value = sigmoid.height;
                else if (param == "a")
                    value = sigmoid.direction;
                else if (param == "s")
                    value = sigmoid.shift;
                else if (param == "x0")
                    value = sigmoid.center;
                else if (param == "w")
                    value = sigmoid.width;
                else
                    return false;
                return true;
            }

            const size_t separator = key.find('.', PrefixMeasures.size());
            if (separator == std::string::npos)
                return false;

            const std::string measureName =
                key.substr(PrefixMeasures.size(), separator - PrefixMeasures.size());
            const auto measure = magic_enum::enum_cast<OFIQ::QualityMeasure>(measureName);
            if (!measure.has_value())
                return false;

            const auto it = m_config.measures.find(measure.value());
            if (it == m_config.measures.end())
                return false;

            const std::string paramName = key.substr(separator + 1);
            const auto param = it->second.params.find(paramName);
            if (param == it->second.params.end())
                return false;

            value = param->second;
            return true;
        }

        return false;
    }

    bool InMemoryConfiguration::GetBool(const std::string& key) const
    {
        bool value = false;
        if (!GetBool(key, value))
            throw OFIQError(OFIQ::ReturnCode::MissingConfigParamError, "Required parameter missing: " + key);
        return value;
    }

    std::string InMemoryConfiguration::GetString(const std::string& key) const
    {
        std::string value;
        if (!GetString(key, value))
            throw OFIQError(OFIQ::ReturnCode::MissingConfigParamError, "Required parameter missing: " + key);
        return value;
    }

    double InMemoryConfiguration::GetNumber(const std::string& key) const
    {
        double value = 0.0;
        if (!GetNumber(key, value))
            throw OFIQError(OFIQ::ReturnCode::MissingConfigParamError, "Required parameter missing: " + key);
        return value;
    }

    std::string InMemoryConfiguration::GetFullPath(const std::string& key) const
    {
        std::filesystem::path path(GetString(key));
        if (path.is_absolute())
            return path.lexically_normal().string();
        return (std::filesystem::path(getDataDir()) / path).lexically_normal().string();
    }
}
