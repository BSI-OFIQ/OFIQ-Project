#pragma once

#include "Configuration.h"
#include "detectors.h"
#include <opencv2/dnn.hpp>

// namespace cv {namespace dnn {class Net;}}

namespace FACEQA_LIB::modules::detectors
{

    class SSDFaceDetector : public FACEQA_LIB::FaceDetectorInterface
    {
    public:
        SSDFaceDetector(const Configuration& config);
        ~SSDFaceDetector() override = default;

    protected:
        std::vector<FACEQA::BoundingBox> UpdateFaces(FACEQA_LIB::Session& session) override;


    private:
        std::shared_ptr<cv::dnn::Net> dnnNet{nullptr};
        // Shrink the image to fit in the quadrat of this size
        int shrinkTo{0};
        static const std::string paramPrototxt;   // { "prototxt" };
        static const std::string paramCaffemodel; // { "caffemodel" };
        bool saveDebugImages = false;
    };
}
