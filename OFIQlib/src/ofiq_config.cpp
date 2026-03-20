/**
 * @file ofiq_config.cpp
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

#include "ofiq_config.h"

namespace OFIQ
{
    namespace
    {
        void setSigmoidParams(SigmoidParams& config, const SigmoidParams& defaults)
        {
            config.height = defaults.height;
            config.center = defaults.center;
            config.width = defaults.width;
            config.direction = defaults.direction;
            config.shift = defaults.shift;
            config.roundToInteger = defaults.roundToInteger;
        }

        void setDetectorConfig(DetectorConfig& config, const defaults::DetectorDefaults& defaults)
        {
            config.modelPath = defaults.modelPath;
            config.prototxtPath = defaults.prototxtPath;
            config.confidenceThreshold = defaults.confidenceThreshold;
            config.minRelativeFaceSize = defaults.minRelativeFaceSize;
            config.padding = defaults.padding;
        }

        void setLandmarksConfig(LandmarksConfig& config, const defaults::LandmarksDefaults& defaults)
        {
            config.modelPath = defaults.modelPath;
        }

        MeasureConfig makeMeasureConfig(const defaults::MeasureDefaults& defaults)
        {
            MeasureConfig config;
            config.enabled = true;
            config.modelPath = defaults.modelPath;
            setSigmoidParams(config.sigmoid, defaults.sigmoid);
            config.params = defaults.params;
            config.additionalModelPaths = defaults.additionalModelPaths;
            return config;
        }
    }

    Configuration Configuration::getDefault() { return Configuration(); }

    Configuration::Configuration()
    {
        modelsBasePath = defaults::ModelsBasePathDefault;
        setDetectorConfig(detector, defaults::DetectorDefault);
        setLandmarksConfig(landmarks, defaults::LandmarksDefault);

        measures[QualityMeasure::HeadPose] = makeMeasureConfig(defaults::HeadPoseDefault);
        measures[QualityMeasure::InterEyeDistance] =
            makeMeasureConfig(defaults::InterEyeDistanceDefault);
        measures[QualityMeasure::NoHeadCoverings] =
            makeMeasureConfig(defaults::NoHeadCoveringsDefault);
        measures[QualityMeasure::BackgroundUniformity] =
            makeMeasureConfig(defaults::BackgroundUniformityDefault);
        measures[QualityMeasure::DynamicRange] = makeMeasureConfig(defaults::DynamicRangeDefault);
        measures[QualityMeasure::ExpressionNeutrality] =
            makeMeasureConfig(defaults::ExpressionNeutralityDefault);
        measures[QualityMeasure::EyesVisible] = makeMeasureConfig(defaults::EyesVisibleDefault);
        measures[QualityMeasure::EyesOpen] = makeMeasureConfig(defaults::EyesOpenDefault);
        measures[QualityMeasure::FaceOcclusionPrevention] =
            makeMeasureConfig(defaults::FaceOcclusionPreventionDefault);
        measures[QualityMeasure::CropOfTheFaceImage] =
            makeMeasureConfig(defaults::CropOfTheFaceImageDefault);
        measures[QualityMeasure::IlluminationUniformity] =
            makeMeasureConfig(defaults::IlluminationUniformityDefault);
        measures[QualityMeasure::MouthOcclusionPrevention] =
            makeMeasureConfig(defaults::MouthOcclusionPreventionDefault);
        measures[QualityMeasure::MouthClosed] = makeMeasureConfig(defaults::MouthClosedDefault);
        measures[QualityMeasure::OverExposurePrevention] =
            makeMeasureConfig(defaults::OverExposurePreventionDefault);
        measures[QualityMeasure::Sharpness] = makeMeasureConfig(defaults::SharpnessDefault);
        measures[QualityMeasure::SingleFacePresent] =
            makeMeasureConfig(defaults::SingleFacePresentDefault);
        measures[QualityMeasure::UnderExposurePrevention] =
            makeMeasureConfig(defaults::UnderExposurePreventionDefault);
        measures[QualityMeasure::UnifiedQualityScore] =
            makeMeasureConfig(defaults::UnifiedQualityScoreDefault);
        measures[QualityMeasure::NaturalColour] =
            makeMeasureConfig(defaults::NaturalColourDefault);
        measures[QualityMeasure::CompressionArtifacts] =
            makeMeasureConfig(defaults::CompressionArtifactsDefault);
        measures[QualityMeasure::Luminance] = makeMeasureConfig(defaults::LuminanceDefault);
        measures[QualityMeasure::HeadSize] = makeMeasureConfig(defaults::HeadSizeDefault);
    }

    Configuration Configuration::getMinimal()
    {
        Configuration config;
        for (auto& [measure, measureConfig] : config.measures)
            measureConfig.enabled = false;

        for (const auto measure : defaults::MinimalEnabledMeasuresDefault)
            config.setMeasureEnabled(measure, true);

        return config;
    }

    void Configuration::setMeasureEnabled(QualityMeasure measure, bool enabled)
    {
        measures[measure].enabled = enabled;
    }
}
