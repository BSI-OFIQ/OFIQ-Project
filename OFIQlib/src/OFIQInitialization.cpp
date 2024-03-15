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
        const Configuration& configuration,
        OFIQ_LIB::Session& session)
    {
        std::vector<std::unique_ptr<Measure>> measure_instances;
        for (auto m : measures)
        {
            measure_instances.emplace_back(
                MeasureFactory::CreateMeasure(
                    m, configuration, session));
        }
        return measure_instances;
    }

    std::unique_ptr<Executor> OFIQImpl::CreateExecutor(Session& session)
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
            measures, *config, session));
    }

    void OFIQImpl::CreateNetworks()
    {
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
            return std::make_shared<FaceParsing>(*config);
        };

        auto getFaceOcclusionExtractor =
            [&]() -> std::shared_ptr<SegmentationExtractorInterface>
        {
            return std::make_shared<FaceOcclusionSegmentation>(*config);
        };

        auto getPoseEstimator =
            [&]() -> std::shared_ptr<PoseEstimatorInterface>
        {
            return std::make_shared < HeadPose3DDFAV2 > (*config);
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