#include "AllDetectors.h"
#include "AllLandmarks.h"
#include "AllMeasures.h"
#include "faceqa_lib_impl.h"
#include "FaceQaError.h"
#include "NeuronalNetworkContainer.h"
#include <boost/di.hpp>
#include <magic_enum.hpp>

namespace FACEQA_LIB
{
    namespace di = boost::di;
    using namespace modules::measures;


    enum class measure_classes
    {
        NONE,
        TotalFacesPresent,
        DynamicRange,
        MouthOpen
    };

    measure_classes MagicEnumFromString(const std::string& key)
    {
        return magic_enum::enum_cast<measure_classes>(key).value_or(measure_classes::NONE);
    }


    Executor FaceQAImpl::CreateExecutor(Session& session)
    {

        bool useThemAll = false;
        std::vector<std::string> requiredMeasurs; 
        if (!config->GetStringList("limit_to_measures", requiredMeasurs)) {
            useThemAll = true;
        }

        if (requiredMeasurs.empty()) {
            useThemAll = true;
        }

        if (useThemAll)
        {
            auto injector = di::make_injector(
                di::bind<Session>.to(session),
                di::bind<Configuration>.to(*config),
                di::bind<FaceDetectorInterface>.to(*(networks->faceDetector)),
                di::bind<FaceLandmarkExtractorInterface>.to(*(networks->landmarkExtractor)));


            return injector.create<Executor>();
        }
        else {


            auto dynamic_bindings = [&](measure_classes& id)
            {
                return di::make_injector(
                    di::bind<Session>.to(session),
                    di::bind<Configuration>.to(*config),
                    di::bind<FaceDetectorInterface>.to(*(networks->faceDetector)),
                    di::bind<FaceLandmarkExtractorInterface>.to(*(networks->landmarkExtractor)),
                    /*<<bind `interface` to lazy lambda expression>>*/
                    di::bind<Measure>().to(
                        [&](const auto& injector) -> std::unique_ptr<Measure>
                        {
                            switch (id)
                            {
                            default:
                                return nullptr;
                            case measure_classes::TotalFacesPresent:
                                return injector
                                    .template create<std::unique_ptr<TotalFacesPresent>>();
                            case measure_classes::DynamicRange:
                                return injector.template create<std::unique_ptr<DynamicRange>>();
                            case measure_classes::MouthOpen:
                                return injector.template create<std::unique_ptr<MouthOpen>>();
                            }
                            return nullptr;
                        }));
            };

            std::vector<std::unique_ptr<Measure>> entries;

            for (auto entry : requiredMeasurs)
            {
                measure_classes id = MagicEnumFromString(entry);

                auto injector = di::make_injector(dynamic_bindings(id));
                auto measure_instance = injector.create<std::unique_ptr<Measure>>();
                if (nullptr != measure_instance) {
                    entries.emplace_back(std::move(measure_instance));
                }
                
            }

            Executor computemeasures(std::move(entries));

            return computemeasures;
        }

    }

    using namespace modules::detectors;
    using namespace modules::landmarks;

    static const std::string faceDetectorConfig = "faceDetection.provider";
    static const std::string faceLandmarksConfig = "faceLandmarks.provider";

    enum class providers
    {
        NONE,
        ssd_facedetect,
        dlib,
        bisenet,
        DenseHeadPoseEstimation,
        mediapipe,
        adnet
    };

    providers ToEnum(const std::string& key)
    {
        return magic_enum::enum_cast<providers>(key).value_or(providers::NONE);
    }

    void FaceQAImpl::CreateNetworks()
    {
        const auto detectorKey = config->GetString(faceDetectorConfig);
        const auto detector = ToEnum(detectorKey);
        const auto configureFaceDetector =
            [&](const auto& injector) -> std::shared_ptr<FaceDetectorInterface>
        {
            switch (detector)
            {
            case providers::ssd_facedetect:
                return injector.template create<std::shared_ptr<SSDFaceDetector>>();
            default:
                throw FaceQaError(
                    FACEQA::ReturnCode::NotImplemented,
                    "face detector not implemented: " + detectorKey);
            }
        };

        const auto landmarksKey = config->GetString(faceLandmarksConfig);
        const auto landmarks = ToEnum(landmarksKey);
        const auto configureLandmarkExtractor =
            [&](const auto& injector) -> std::shared_ptr<FaceLandmarkExtractorInterface>
        {
            switch (landmarks)
            {
            case providers::adnet:
                return injector.template create<std::shared_ptr<ADNetFaceLandmarkExtractor>>();
            default:
                throw FaceQaError(
                    FACEQA::ReturnCode::NotImplemented,
                    "landmark extractor not implemented: " + landmarksKey);
            }
        };



        const auto injector = di::make_injector(
            di::bind<Configuration>.to(*config),
            di::bind<FaceDetectorInterface>.to(
                [&](const auto& injector) -> std::shared_ptr<FaceDetectorInterface>
                { return configureFaceDetector(injector); }),
            di::bind<FaceLandmarkExtractorInterface>.to(
                [&](const auto& injector) -> std::shared_ptr<FaceLandmarkExtractorInterface>
                { return configureLandmarkExtractor(injector); }));

        networks.release();
        networks =
            std::make_unique<NeuronalNetworkContainer>(injector.create<NeuronalNetworkContainer>());
    }
}