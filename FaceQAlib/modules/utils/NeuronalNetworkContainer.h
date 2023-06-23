#pragma once
#include "detectors.h"
#include "landmarks.h"

 namespace FACEQA_LIB
{
    struct NeuronalNetworkContainer
    {
        NeuronalNetworkContainer(
            std::shared_ptr<FaceDetectorInterface> faceDetector,
            std::shared_ptr<FaceLandmarkExtractorInterface> landmarkExtractor)
            : faceDetector{faceDetector},
              landmarkExtractor{landmarkExtractor}
        {
        }

        std::shared_ptr<FaceDetectorInterface> faceDetector;
        std::shared_ptr<FaceLandmarkExtractorInterface> landmarkExtractor;
    };
}