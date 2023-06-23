/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include "Configuration.h"
#include "Executor.h"
#include "faceqa_lib_impl.h"
#include "FaceQaError.h"

using namespace std;
using namespace FACEQA;
using namespace FACEQA_LIB;
using Executor = FACEQA_LIB::modules::measures::Executor;

FaceQAImpl::FaceQAImpl() {}

FaceQAImpl::~FaceQAImpl() {}

ReturnStatus FaceQAImpl::initialize(const std::string& configDir, const std::string& configValue)
{
    try
    {
        this->config = std::make_unique<Configuration>(configDir, configValue);
        CreateNetworks();
    }
    catch (const FaceQaError & ex)
    {
        return {ex.whatCode(), ex.what()};
    }
    catch (const std::exception & ex)
    {
        return {ReturnCode::UnknownError, ex.what()};
    }

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus FaceQAImpl::scalarQuality(const FACEQA::Image& face, double& quality)
{
    // TODO: calculate a scalar value for image quality based on particular
    // image quality measurements
    FaceImageQualityAssessment assessments;
    auto result = vectorQuality(face, assessments);
    if (result.code != ReturnCode::Success)
        return result;

    // just as an example - the scalarQuality is an average of all valid scalar measurements
    double sumScalars = 0;
    int numScalars = 0;
    for (auto const& aResult : assessments.qAssessments)
    {
        if (aResult.second.scalar != -1)
        {
            sumScalars += aResult.second.scalar;
            numScalars++;
        }
    }
    quality = sumScalars / numScalars;

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus FaceQAImpl::vectorQuality(
    const FACEQA::Image& image, FACEQA::FaceImageQualityAssessment& assessments)
{
    auto session = Session(image, assessments);

    try
    {
        CreateExecutor(session).ExecuteAll();
    }
    catch (const FaceQaError& e)
    {
        return { e.whatCode(), e.what() };
    }
    catch (const std::exception& e)
    {
        return {ReturnCode::UnknownError, e.what()};
    }

    return ReturnStatus(ReturnCode::Success);
}

FACEQA_EXPORT std::shared_ptr<Interface> Interface::getImplementation()
{
    return std::make_shared<FaceQAImpl>();
}
