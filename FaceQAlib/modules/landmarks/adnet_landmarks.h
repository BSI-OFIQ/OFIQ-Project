#pragma once

#include <memory>
#include "Configuration.h"
#include "detectors.h"
#include "landmarks.h"


namespace FACEQA_LIB::modules::landmarks
{
    
    
    class ADNetFaceLandmarkExtractorImpl;
    class ADNetFaceLandmarkExtractor : public FaceLandmarkExtractorInterface
    {
    public:
        ADNetFaceLandmarkExtractor(const Configuration& config, FaceDetectorInterface& detector);

        ~ADNetFaceLandmarkExtractor() override;

    protected:
        FACEQA::FaceLandmarks updateLandmarks(FACEQA_LIB::Session& session) override;


    private:
        
        FaceDetectorInterface& detector;
        
        std::unique_ptr<ADNetFaceLandmarkExtractorImpl> landmarkExtractor;
        bool saveDebugImages = false;
    };
}
