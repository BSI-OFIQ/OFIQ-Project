/**
 * @file ofiq_config.h
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
 * @brief Typed OFIQ configuration API.
 * @note Defaults of this typed configuration API must stay aligned with the
 * defaults in the JAXN configuration file.
 * @author OFIQ development team
 */

#pragma once

#include <ofiq_export.h>
#include <ofiq_structs.h>

#include <array>
#include <map>
#include <string>

namespace OFIQ
{
    struct SigmoidParams
    {
        double height;
        double center;
        double width;
        double direction;
        double shift;
        bool roundToInteger;
    };

    struct DetectorConfig
    {
        std::string modelPath;
        std::string prototxtPath;
        double confidenceThreshold;
        double minRelativeFaceSize;
        double padding;
    };

    struct LandmarksConfig
    {
        std::string modelPath;
    };

    struct MeasureConfig
    {
        bool enabled;
        std::string modelPath;
        SigmoidParams sigmoid;
        std::map<std::string, double> params;
        std::map<std::string, std::string> additionalModelPaths;
    };

    namespace defaults
    {
        inline constexpr const char* ModelsBasePathDefault = "./data/models";

        inline const SigmoidParams SigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 100.0;
            defaults.center = 0.0;
            defaults.width = 1.0;
            defaults.direction = 1.0;
            defaults.shift = 0.0;
            defaults.roundToInteger = true;
            return defaults;
        }();

        struct DetectorDefaults
        {
            const char* modelPath;
            const char* prototxtPath;
            double confidenceThreshold;
            double minRelativeFaceSize;
            double padding;
        };

        inline const DetectorDefaults DetectorDefault = [] {
            DetectorDefaults defaults{};
            defaults.modelPath = "face_detection/ssd_facedetect.caffemodel";
            defaults.prototxtPath = "face_detection/ssd_facedetect.prototxt.txt";
            defaults.confidenceThreshold = 0.4;
            defaults.minRelativeFaceSize = 0.05;
            defaults.padding = 0.2;
            return defaults;
        }();

        struct LandmarksDefaults
        {
            const char* modelPath;
        };

        inline const LandmarksDefaults LandmarksDefault = [] {
            LandmarksDefaults defaults{};
            defaults.modelPath = "face_landmark_estimation/ADNet.onnx";
            return defaults;
        }();

        struct MeasureDefaults
        {
            const char* modelPath;
            SigmoidParams sigmoid;
            std::map<std::string, double> params;
            std::map<std::string, std::string> additionalModelPaths;
        };

        inline const MeasureDefaults HeadPoseDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "head_pose_estimation/mb1_120x120.onnx";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams InterEyeDistanceSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 100.0;
            defaults.center = 70.0;
            defaults.width = 20.0;
            defaults.direction = 0.0;
            defaults.shift = 1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults InterEyeDistanceDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = InterEyeDistanceSigmoidParamsDefault;
            return defaults;
        }();

        inline const MeasureDefaults NoHeadCoveringsDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            defaults.params["T0"] = 0.0;
            defaults.params["T1"] = 0.95;
            defaults.params["w"] = 0.1;
            defaults.params["x0"] = 0.02;
            return defaults;
        }();

        inline const SigmoidParams BackgroundUniformitySigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 190.0;
            defaults.center = 10.0;
            defaults.width = 100.0;
            defaults.direction = 1.0;
            defaults.shift = -1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults BackgroundUniformityDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = BackgroundUniformitySigmoidParamsDefault;
            return defaults;
        }();

        inline const MeasureDefaults DynamicRangeDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams ExpressionNeutralitySigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 100.0;
            defaults.center = -5000.0;
            defaults.width = 5000.0;
            defaults.direction = 0.0;
            defaults.shift = 1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults ExpressionNeutralityDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = ExpressionNeutralitySigmoidParamsDefault;
            defaults.additionalModelPaths["cnn1_model_path"] =
                "expression_neutrality/hsemotion/enet_b0_8_best_vgaf_embed_zeroed.onnx";
            defaults.additionalModelPaths["cnn2_model_path"] =
                "expression_neutrality/hsemotion/enet_b2_8_embed_zeroed.onnx";
            defaults.additionalModelPaths["adaboost_model_path"] =
                "expression_neutrality/grimmer/hse_1_2_C_adaboost.yml.gz";
            return defaults;
        }();

        inline const MeasureDefaults EyesVisibleDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams EyesOpenSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 100.0;
            defaults.center = 0.02;
            defaults.width = 0.01;
            defaults.direction = 0.0;
            defaults.shift = 1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults EyesOpenDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = EyesOpenSigmoidParamsDefault;
            return defaults;
        }();

        inline const MeasureDefaults FaceOcclusionPreventionDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();
        inline const MeasureDefaults CropOfTheFaceImageDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();
        inline const MeasureDefaults IlluminationUniformityDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();
        inline const MeasureDefaults MouthOcclusionPreventionDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams MouthClosedSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 100.0;
            defaults.center = 0.2;
            defaults.width = 0.06;
            defaults.direction = 1.0;
            defaults.shift = -1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults MouthClosedDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = MouthClosedSigmoidParamsDefault;
            return defaults;
        }();

        inline const MeasureDefaults OverExposurePreventionDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams SharpnessSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 1.0;
            defaults.center = -20.0;
            defaults.width = 15.0;
            defaults.direction = -14.0;
            defaults.shift = 115.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults SharpnessDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "sharpness/face_sharpness_rtree.xml.gz";
            defaults.sigmoid = SharpnessSigmoidParamsDefault;
            defaults.params["face_region_alpha"] = 0.0;
            defaults.params["use_aligned_landmarks"] = 0.0;
            return defaults;
        }();

        inline const MeasureDefaults SingleFacePresentDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams UnderExposurePreventionSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 120.0;
            defaults.center = 0.92;
            defaults.width = 0.05;
            defaults.direction = 0.832;
            defaults.shift = -1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults UnderExposurePreventionDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = UnderExposurePreventionSigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams UnifiedQualityScoreSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 100.0;
            defaults.center = 23.0;
            defaults.width = 2.6;
            defaults.direction = 0.0;
            defaults.shift = 1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults UnifiedQualityScoreDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "unified_quality_score/magface_iresnet50_norm.onnx";
            defaults.sigmoid = UnifiedQualityScoreSigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams NaturalColourSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 200.0;
            defaults.center = 0.0;
            defaults.width = 10.0;
            defaults.direction = 1.0;
            defaults.shift = -1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults NaturalColourDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = NaturalColourSigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams CompressionArtifactsSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 1.0;
            defaults.center = 0.3308;
            defaults.width = 0.092;
            defaults.direction = -0.0278;
            defaults.shift = 103.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults CompressionArtifactsDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "no_compression_artifacts/ssim_248_model.onnx";
            defaults.sigmoid = CompressionArtifactsSigmoidParamsDefault;
            defaults.params["crop"] = 184.0;
            defaults.params["dim"] = 248.0;
            return defaults;
        }();

        inline const MeasureDefaults LuminanceDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = SigmoidParamsDefault;
            return defaults;
        }();

        inline const SigmoidParams HeadSizeSigmoidParamsDefault = [] {
            SigmoidParams defaults{};
            defaults.height = 200.0;
            defaults.center = 0.0;
            defaults.width = 0.05;
            defaults.direction = 1.0;
            defaults.shift = -1.0;
            defaults.roundToInteger = true;
            return defaults;
        }();
        inline const MeasureDefaults HeadSizeDefault = [] {
            MeasureDefaults defaults{};
            defaults.modelPath = "";
            defaults.sigmoid = HeadSizeSigmoidParamsDefault;
            return defaults;
        }();

        inline const std::array<QualityMeasure, 4> MinimalEnabledMeasuresDefault = {
            QualityMeasure::UnifiedQualityScore,
            QualityMeasure::SingleFacePresent,
            QualityMeasure::Sharpness,
            QualityMeasure::EyesOpen,
        };
    }

    struct Configuration
    {
        OFIQ_EXPORT Configuration();

        std::string modelsBasePath;
        DetectorConfig detector;
        LandmarksConfig landmarks;
        std::map<QualityMeasure, MeasureConfig> measures;

        static OFIQ_EXPORT Configuration getDefault();
        static OFIQ_EXPORT Configuration getMinimal();

        OFIQ_EXPORT void setMeasureEnabled(QualityMeasure measure, bool enabled);
    };
}
