#ifndef FACEQA_LIB_H
#define FACEQA_LIB_H

#include <cstdint>
#include <string>
#include <vector>

#include <faceqa_structs.h>

#ifdef _WIN32
#    ifdef FACEQA_EXPORTS
#        define FACEQA_EXPORT __declspec(dllexport)
#    else
#        define FACEQA_EXPORT __declspec(dllimport)
#    endif
#else
#    define FACEQA_EXPORT
#endif

namespace FACEQA_LIB
{

    /**
     * @brief
     * The interface to FACE QA implementation
     *
     * @details
     * Implement this interface by sub-classing this class and implementing each method therein.
     */
    class Interface
    {
    public:
        virtual ~Interface() {}

        /**
         * @brief This function initializes the implementation under test.
         * The implementation under test should set all parameters.
         *
         * @param[in] configDir
         * A read-only directory containing any configuration
         * parameters or run-time data files. The names of the
         * files in this directory are hardwired in the implementation and are
         * unrestricted.
         * @param[in] configValue
         * An optional string value encoding developer-specific configuration parameters         */
        virtual FACEQA::ReturnStatus
            initialize(const std::string& configDir, const std::string& configValue) = 0;

        /**
         * @brief This function takes an image and outputs a quality scalar.
         *
         * @param[in] face
         * Single face image
         * @param[out] quality
         * A scalar value assessment of image quality.  The legal values are
         * [0,100]  So, a low value indicates high expected FNMR.
         * A value of -1.0 indicates a failed attempt to calculate a quality
         * score or the value is unassigned.
         */
        virtual FACEQA::ReturnStatus scalarQuality(const FACEQA::Image& face, double& quality) = 0;

        /**
         * @brief  This function takes an image and outputs
         * face location and quality information. The quality assessment
         * should be performed on the largest detected face.
         *
         * @param[in] image
         * Single face image
         *
         * @param[out] assessments
         * An ImageQualityAssessments structure.
         * The implementation should populate
         * 1) the bounding box and
         * 2) those items in the QualityAssessments object that the
         * developer chooses to implement
         * 3) face landmarks
         */
        virtual FACEQA::ReturnStatus vectorQuality(
            const FACEQA::Image& image, FACEQA::FaceImageQualityAssessment& assessments) = 0;

        /**
         * @brief
         * Factory method to return a managed pointer to the Interface object.
         * @details
         * This function is implemented by the submitted library and must return
         * a managed pointer to the Interface object.
         *
         * This function MUST be implemented.
         *
         * @note
         * A possible implementation might be:
         * return (std::make_shared<Implementation>());
         */
        FACEQA_EXPORT static std::shared_ptr<Interface> getImplementation();
    };
}

#endif /* FACEQA_LIB_H */
