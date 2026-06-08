/**
 * @file test_config_defaults_jaxn.cpp
 *
 * @copyright Copyright (c) 2024, 2025, 2026 Federal Office for Information Security, Germany
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
 */

#include <ofiq_config.h>

#include "JaxnConfiguration.h"

#include <gtest/gtest.h>
#include <magic_enum.hpp>

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    const fs::path ConfigDir = fs::weakly_canonical(fs::path("../../../data"));
    const fs::path ModelsDir = (ConfigDir / "models").lexically_normal();

    void expectNumberEquals(
        const OFIQ_LIB::JaxnConfiguration& jaxnConfig,
        const std::string& key,
        double expectedValue)
    {
        double actualValue = 0.0;
        ASSERT_TRUE(jaxnConfig.GetNumber(key, actualValue)) << key;
        EXPECT_DOUBLE_EQ(actualValue, expectedValue) << key;
    }

    void expectBoolEquals(
        const OFIQ_LIB::JaxnConfiguration& jaxnConfig,
        const std::string& key,
        bool expectedValue)
    {
        bool actualValue = false;
        ASSERT_TRUE(jaxnConfig.GetBool(key, actualValue)) << key;
        EXPECT_EQ(actualValue, expectedValue) << key;
    }

    void expectModelPathEquals(
        const OFIQ_LIB::JaxnConfiguration& jaxnConfig,
        const std::string& key,
        const std::string& relativeModelPath)
    {
        const std::string expectedPath = (ModelsDir / relativeModelPath).lexically_normal().string();
        EXPECT_EQ(jaxnConfig.GetFullPath(key), expectedPath) << key;
    }

    std::vector<std::string> normalizeMeasureNames(std::vector<std::string> measureNames)
    {
        std::sort(measureNames.begin(), measureNames.end());
        measureNames.erase(
            std::unique(measureNames.begin(), measureNames.end()),
            measureNames.end());
        return measureNames;
    }
}

TEST(DefaultConfiguration, MatchesJaxnDefaults)
{
    ASSERT_TRUE(fs::exists(ConfigDir)) << ConfigDir.string();

    const OFIQ_LIB::JaxnConfiguration jaxnConfig(ConfigDir.string(), "ofiq_config.jaxn");
    const OFIQ::Configuration defaultConfig = OFIQ::Configuration::getDefault();

    std::vector<std::string> enabledMeasures;
    for (const auto& [measure, measureConfig] : defaultConfig.measures)
    {
        if (measureConfig.enabled)
            enabledMeasures.push_back(static_cast<std::string>(magic_enum::enum_name(measure)));
    }

    std::vector<std::string> jaxnMeasures;
    ASSERT_TRUE(jaxnConfig.GetStringList("measures", jaxnMeasures));

    // The JAXN file currently contains a duplicate NoHeadCoverings entry.
    EXPECT_EQ(
        normalizeMeasureNames(enabledMeasures),
        normalizeMeasureNames(jaxnMeasures));

    expectModelPathEquals(
        jaxnConfig,
        "params.detector.ssd.model_path",
        OFIQ::defaults::DetectorDefault.modelPath);
    expectModelPathEquals(
        jaxnConfig,
        "params.detector.ssd.prototxt_path",
        OFIQ::defaults::DetectorDefault.prototxtPath);
    expectNumberEquals(
        jaxnConfig,
        "params.detector.ssd.confidence_thr",
        OFIQ::defaults::DetectorDefault.confidenceThreshold);
    expectNumberEquals(
        jaxnConfig,
        "params.detector.ssd.min_rel_face_size",
        OFIQ::defaults::DetectorDefault.minRelativeFaceSize);
    expectNumberEquals(
        jaxnConfig,
        "params.detector.ssd.padding",
        OFIQ::defaults::DetectorDefault.padding);

    expectModelPathEquals(
        jaxnConfig,
        "params.landmarks.ADNet.model_path",
        OFIQ::defaults::LandmarksDefault.modelPath);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.BackgroundUniformity.Sigmoid.h",
        OFIQ::defaults::BackgroundUniformitySigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.BackgroundUniformity.Sigmoid.a",
        OFIQ::defaults::BackgroundUniformitySigmoidParamsDefault.direction);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.BackgroundUniformity.Sigmoid.s",
        OFIQ::defaults::BackgroundUniformitySigmoidParamsDefault.shift);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.BackgroundUniformity.Sigmoid.x0",
        OFIQ::defaults::BackgroundUniformitySigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.BackgroundUniformity.Sigmoid.w",
        OFIQ::defaults::BackgroundUniformitySigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.BackgroundUniformity.Sigmoid.round",
        OFIQ::defaults::BackgroundUniformitySigmoidParamsDefault.roundToInteger);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnderExposurePrevention.Sigmoid.h",
        OFIQ::defaults::UnderExposurePreventionSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnderExposurePrevention.Sigmoid.a",
        OFIQ::defaults::UnderExposurePreventionSigmoidParamsDefault.direction);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnderExposurePrevention.Sigmoid.s",
        OFIQ::defaults::UnderExposurePreventionSigmoidParamsDefault.shift);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnderExposurePrevention.Sigmoid.x0",
        OFIQ::defaults::UnderExposurePreventionSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnderExposurePrevention.Sigmoid.w",
        OFIQ::defaults::UnderExposurePreventionSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.UnderExposurePrevention.Sigmoid.round",
        OFIQ::defaults::UnderExposurePreventionSigmoidParamsDefault.roundToInteger);

    expectModelPathEquals(
        jaxnConfig,
        "params.measures.Sharpness.model_path",
        OFIQ::defaults::SharpnessDefault.modelPath);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.Sharpness.face_region_alpha",
        OFIQ::defaults::SharpnessDefault.params.at("face_region_alpha"));
    expectBoolEquals(
        jaxnConfig,
        "params.measures.Sharpness.use_aligned_landmarks",
        OFIQ::defaults::SharpnessDefault.params.at("use_aligned_landmarks") != 0.0);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.Sharpness.Sigmoid.h",
        OFIQ::defaults::SharpnessSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.Sharpness.Sigmoid.a",
        OFIQ::defaults::SharpnessSigmoidParamsDefault.direction);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.Sharpness.Sigmoid.s",
        OFIQ::defaults::SharpnessSigmoidParamsDefault.shift);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.Sharpness.Sigmoid.x0",
        OFIQ::defaults::SharpnessSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.Sharpness.Sigmoid.w",
        OFIQ::defaults::SharpnessSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.Sharpness.Sigmoid.round",
        OFIQ::defaults::SharpnessSigmoidParamsDefault.roundToInteger);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.NaturalColor.Sigmoid.h",
        OFIQ::defaults::NaturalColourSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.NaturalColor.Sigmoid.a",
        OFIQ::defaults::NaturalColourSigmoidParamsDefault.direction);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.NaturalColor.Sigmoid.s",
        OFIQ::defaults::NaturalColourSigmoidParamsDefault.shift);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.NaturalColor.Sigmoid.x0",
        OFIQ::defaults::NaturalColourSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.NaturalColor.Sigmoid.w",
        OFIQ::defaults::NaturalColourSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.NaturalColor.Sigmoid.round",
        OFIQ::defaults::NaturalColourSigmoidParamsDefault.roundToInteger);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.EyesOpen.Sigmoid.h",
        OFIQ::defaults::EyesOpenSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.EyesOpen.Sigmoid.x0",
        OFIQ::defaults::EyesOpenSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.EyesOpen.Sigmoid.w",
        OFIQ::defaults::EyesOpenSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.EyesOpen.Sigmoid.round",
        OFIQ::defaults::EyesOpenSigmoidParamsDefault.roundToInteger);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.MouthClosed.Sigmoid.h",
        OFIQ::defaults::MouthClosedSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.MouthClosed.Sigmoid.a",
        OFIQ::defaults::MouthClosedSigmoidParamsDefault.direction);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.MouthClosed.Sigmoid.s",
        OFIQ::defaults::MouthClosedSigmoidParamsDefault.shift);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.MouthClosed.Sigmoid.x0",
        OFIQ::defaults::MouthClosedSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.MouthClosed.Sigmoid.w",
        OFIQ::defaults::MouthClosedSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.MouthClosed.Sigmoid.round",
        OFIQ::defaults::MouthClosedSigmoidParamsDefault.roundToInteger);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.InterEyeDistance.Sigmoid.h",
        OFIQ::defaults::InterEyeDistanceSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.InterEyeDistance.Sigmoid.x0",
        OFIQ::defaults::InterEyeDistanceSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.InterEyeDistance.Sigmoid.w",
        OFIQ::defaults::InterEyeDistanceSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.InterEyeDistance.Sigmoid.round",
        OFIQ::defaults::InterEyeDistanceSigmoidParamsDefault.roundToInteger);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.HeadSize.Sigmoid.h",
        OFIQ::defaults::HeadSizeSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.HeadSize.Sigmoid.a",
        OFIQ::defaults::HeadSizeSigmoidParamsDefault.direction);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.HeadSize.Sigmoid.s",
        OFIQ::defaults::HeadSizeSigmoidParamsDefault.shift);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.HeadSize.Sigmoid.x0",
        OFIQ::defaults::HeadSizeSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.HeadSize.Sigmoid.w",
        OFIQ::defaults::HeadSizeSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.HeadSize.Sigmoid.round",
        OFIQ::defaults::HeadSizeSigmoidParamsDefault.roundToInteger);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.NoHeadCoverings.T0",
        OFIQ::defaults::NoHeadCoveringsDefault.params.at("T0"));
    expectNumberEquals(
        jaxnConfig,
        "params.measures.NoHeadCoverings.T1",
        OFIQ::defaults::NoHeadCoveringsDefault.params.at("T1"));
    expectNumberEquals(
        jaxnConfig,
        "params.measures.NoHeadCoverings.w",
        OFIQ::defaults::NoHeadCoveringsDefault.params.at("w"));
    expectNumberEquals(
        jaxnConfig,
        "params.measures.NoHeadCoverings.x0",
        OFIQ::defaults::NoHeadCoveringsDefault.params.at("x0"));

    expectModelPathEquals(
        jaxnConfig,
        "params.measures.HeadPose.model_path",
        OFIQ::defaults::HeadPoseDefault.modelPath);

    expectNumberEquals(
        jaxnConfig,
        "params.measures.FaceRegion.alpha",
        0.0);

    expectModelPathEquals(
        jaxnConfig,
        "params.measures.UnifiedQualityScore.model_path",
        OFIQ::defaults::UnifiedQualityScoreDefault.modelPath);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnifiedQualityScore.Sigmoid.h",
        OFIQ::defaults::UnifiedQualityScoreSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnifiedQualityScore.Sigmoid.x0",
        OFIQ::defaults::UnifiedQualityScoreSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.UnifiedQualityScore.Sigmoid.w",
        OFIQ::defaults::UnifiedQualityScoreSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.UnifiedQualityScore.Sigmoid.round",
        OFIQ::defaults::UnifiedQualityScoreSigmoidParamsDefault.roundToInteger);

    expectModelPathEquals(
        jaxnConfig,
        "params.measures.ExpressionNeutrality.cnn1_model_path",
        OFIQ::defaults::ExpressionNeutralityDefault.additionalModelPaths.at("cnn1_model_path"));
    expectModelPathEquals(
        jaxnConfig,
        "params.measures.ExpressionNeutrality.cnn2_model_path",
        OFIQ::defaults::ExpressionNeutralityDefault.additionalModelPaths.at("cnn2_model_path"));
    expectModelPathEquals(
        jaxnConfig,
        "params.measures.ExpressionNeutrality.adaboost_model_path",
        OFIQ::defaults::ExpressionNeutralityDefault.additionalModelPaths.at("adaboost_model_path"));
    expectNumberEquals(
        jaxnConfig,
        "params.measures.ExpressionNeutrality.Sigmoid.h",
        OFIQ::defaults::ExpressionNeutralitySigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.ExpressionNeutrality.Sigmoid.x0",
        OFIQ::defaults::ExpressionNeutralitySigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.ExpressionNeutrality.Sigmoid.w",
        OFIQ::defaults::ExpressionNeutralitySigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.ExpressionNeutrality.Sigmoid.round",
        OFIQ::defaults::ExpressionNeutralitySigmoidParamsDefault.roundToInteger);

    expectModelPathEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.model_path",
        OFIQ::defaults::CompressionArtifactsDefault.modelPath);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.crop",
        OFIQ::defaults::CompressionArtifactsDefault.params.at("crop"));
    expectNumberEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.dim",
        OFIQ::defaults::CompressionArtifactsDefault.params.at("dim"));
    expectNumberEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.Sigmoid.h",
        OFIQ::defaults::CompressionArtifactsSigmoidParamsDefault.height);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.Sigmoid.a",
        OFIQ::defaults::CompressionArtifactsSigmoidParamsDefault.direction);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.Sigmoid.s",
        OFIQ::defaults::CompressionArtifactsSigmoidParamsDefault.shift);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.Sigmoid.x0",
        OFIQ::defaults::CompressionArtifactsSigmoidParamsDefault.center);
    expectNumberEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.Sigmoid.w",
        OFIQ::defaults::CompressionArtifactsSigmoidParamsDefault.width);
    expectBoolEquals(
        jaxnConfig,
        "params.measures.CompressionArtifacts.Sigmoid.round",
        OFIQ::defaults::CompressionArtifactsSigmoidParamsDefault.roundToInteger);
}
