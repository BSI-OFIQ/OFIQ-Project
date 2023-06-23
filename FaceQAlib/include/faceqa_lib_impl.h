#ifndef FACEQA_LIB_IMPL_H
#define FACEQA_LIB_IMPL_H

#include "Configuration.h"
#include "Executor.h"
#include "faceqa_lib.h"
#include "NeuronalNetworkContainer.h"

/*
 * Implementation class of the FACEQA_LIB Interface
 */
namespace FACEQA_LIB
{
    class FaceQAImpl : public FACEQA_LIB::Interface
    {
    public:
        FaceQAImpl();
        ~FaceQAImpl() override;

        FACEQA::ReturnStatus
            initialize(const std::string& configDir, const std::string& configValue) override;

        FACEQA::ReturnStatus scalarQuality(const FACEQA::Image& face, double& quality) override;

        FACEQA::ReturnStatus vectorQuality(
            const FACEQA::Image& image, FACEQA::FaceImageQualityAssessment& assessments) override;

    private:
        std::unique_ptr<Configuration> config;
        std::unique_ptr<NeuronalNetworkContainer> networks;

        modules::measures::Executor CreateExecutor(Session& session);
        void CreateNetworks();
    };
}

#endif /* FACEQA_LIB_IMPL_H */
