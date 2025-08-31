/**
 * @file OFIQInitialization.cpp
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

#include "AllDetectors.h"
#include "AllLandmarks.h"
#include "AllMeasures.h"
#include "AllPoseEstimators.h"
#include "MeasureFactory.h"
#include "ofiq_lib_impl.h"
#include "OFIQError.h"
#include "NeuronalNetworkContainer.h"
#include <magic_enum.hpp>
#include "modules/utils/logging.h"

namespace OFIQ_LIB
{
    using namespace modules::measures;
    using namespace modules::detectors;
    using namespace modules::landmarks;
    using namespace modules::segmentations;
    using namespace modules::poseEstimators;

    std::vector<OFIQ::QualityMeasure> parse_config_measure_names(
        const std::vector<std::string>& measure_names,
        std::vector<std::string>& invalid_names)
    {
        std::vector<OFIQ::QualityMeasure> measures;
        for (auto measure_name : measure_names)
        {
            auto measure = magic_enum::enum_cast<OFIQ::QualityMeasure>(measure_name);
            if (measure.has_value())
                measures.emplace_back(measure.value());
            else
                invalid_names.emplace_back(measure_name);
        }
        return measures;
    }

    std::vector<std::unique_ptr<Measure>> create_measures(
        const std::vector<OFIQ::QualityMeasure>& measures,
        const Configuration& configuration)
    {
        std::vector<std::unique_ptr<Measure>> measure_instances;
        for (auto m : measures)
        {
            measure_instances.emplace_back(
                MeasureFactory::CreateMeasure(
                    m, configuration));
        }
        return measure_instances;
    }

    std::unique_ptr<Executor> OFIQImpl::CreateExecutor()
    {
        std::vector<std::string> requested_measurs;
        if (!config->GetStringList("measures", requested_measurs) ||
            requested_measurs.empty())
        {
            throw OFIQError(
                OFIQ::ReturnCode::NotImplemented,
                "The 'measures' section in config file is missing or empty\n");
        }

        std::vector<std::string> invalid_measure_names;

        std::vector<OFIQ::QualityMeasure> measures =
            parse_config_measure_names(requested_measurs,
                invalid_measure_names);

        for (const auto& m : invalid_measure_names)
        {
            // create log output with level warning
            std::cout << "invalid measure name detected in config file: "
                << m << std::endl;
        }

        // initialise measures
        
        return std::make_unique<Executor>(create_measures(
            measures, *config));
    }

    void OFIQImpl::CreateNetworks()
    {
        // Parse requested measures from config to determine dependencies
        std::vector<std::string> requested_measures_str;
        bool has_measures = config->GetStringList("measures", requested_measures_str) && !requested_measures_str.empty();
        // Info log active measures (configurable)
        {
            std::string msg = "OFIQ init: measures=";
            if (has_measures) {
                for (size_t i=0;i<requested_measures_str.size();++i) {
                    if (i) msg += ",";
                    msg += requested_measures_str[i];
                }
            } else {
                msg += "<none>";
            }
            OFIQ_LIB::logging::log(OFIQ_LIB::logging::LogLevel::Info, msg, "init");
        }

        auto has_any = [&](const std::initializer_list<const char*>& names) -> bool {
            if (!has_measures) return true; // if not specified, be conservative and enable
            for (const auto& n : names) {
                for (const auto& m : requested_measures_str) {
                    if (m == n) return true;
                }
            }
            return false;
        };

        // Dependencies by measure name
        bool need_pose = has_any({
            "HeadPose", "HeadPoseYaw", "HeadPosePitch", "HeadPoseRoll",
            "InterEyeDistance", "EyesVisible"
        });
        bool need_face_parsing = has_any({
            "BackgroundUniformity", "NoHeadCoverings"
        });
        bool need_face_occlusion = has_any({
            "FaceOcclusionPrevention", "MouthOcclusionPrevention", "EyesVisible"
        });

        auto getFaceDetector =
            [&]() -> std::shared_ptr<FaceDetectorInterface>
        {
            return std::make_shared<SSDFaceDetector>(*config);
        };

        auto getLandmarkExtractor =
            [&]() -> std::shared_ptr<FaceLandmarkExtractorInterface>
        {
            return std::make_shared <ADNetFaceLandmarkExtractor> (*config);
        };

        auto getSegmentationExtractor =
            [&]() -> std::shared_ptr<SegmentationExtractorInterface>
        {
            if (need_face_parsing)
                return std::make_shared<FaceParsing>(*config);
            return std::shared_ptr<SegmentationExtractorInterface>{};
        };

        auto getFaceOcclusionExtractor =
            [&]() -> std::shared_ptr<SegmentationExtractorInterface>
        {
            if (need_face_occlusion)
                return std::make_shared<FaceOcclusionSegmentation>(*config);
            return std::shared_ptr<SegmentationExtractorInterface>{};
        };

        auto getPoseEstimator =
            [&]() -> std::shared_ptr<PoseEstimatorInterface>
        {
            if (need_pose)
                return std::make_shared < HeadPose3DDFAV2 > (*config);
            return std::shared_ptr<PoseEstimatorInterface>{};
        };

        networks.release();
        networks = std::make_unique<NeuronalNetworkContainer>(
            getFaceDetector(),
            getLandmarkExtractor(),
            getSegmentationExtractor(),
            getPoseEstimator(),
            getFaceOcclusionExtractor()
            );
    }
}
