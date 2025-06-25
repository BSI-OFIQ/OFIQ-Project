/**
 * @file ofiq_lib.h
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
 * @brief Class describing the interface to the OFIQ.
 * @author OFIQ development team
 */
#ifndef OFIQ_LIB_H
#define OFIQ_LIB_H

#include <cstdint>
#include <string>
#include <vector>

#include <ofiq_structs.h>

#ifdef _WIN32
#    ifdef OFIQ_EXPORTS
#        define OFIQ_EXPORT __declspec(dllexport)
#    else
#        define OFIQ_EXPORT __declspec(dllimport)
#    endif
#else
#    define OFIQ_EXPORT
#endif

/**
 * @brief Namespace for OFIQ API.
 */
namespace OFIQ
{
    enum class PreprocessingResultType
    {
        // Flag to request no preprocessing data
        None = 0x0,

        // Flag to request detected faces
        Faces = 0x1,

        // Flag to request landmarks
        Landmarks = 0x2,

        // Flag to request face parsing segmentation
        Segmentation = 0x4,

        // Flag to request face occlusion mask
        OcclusionMask = 0x8,

        // Flag to request landmarked region
        LandmarkedRegion = 0x10,

        // Mask to request all available pre-processing data
        All = 0x1 + 0x2 + 0x4 + 0x8 + 0x10
    };

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
        /**
         * @brief Default Destructor
         * 
         */
        virtual ~Interface() = default;

        /**
         * @brief This function initializes the implementation under test.
         * The implementation under test should set all parameters.
         *
         * @param[in] configDir
         * string representation of the directory containing the configuration 
         * file specified by
         * configFileName
         * @param[in] configFileName
         * An string value encoding the JAXN configuration file name         
         * @return OFIQ::ReturnStatus indicating if the initialization was successful.
         */
        virtual OFIQ::ReturnStatus
            initialize(const std::string& configDir, const std::string& configFileName) = 0;

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
         * @return OFIQ::ReturnStatus
         */
        virtual OFIQ::ReturnStatus scalarQuality(const OFIQ::Image& face, double& quality) = 0;

        /**
         * @brief  This function takes an image and outputs quality information.
         *
         * @details Implementing functions should be performed on the largest detected face.
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
         * 
         * @return OFIQ::ReturnStatus
         */
        virtual OFIQ::ReturnStatus vectorQuality(
            const OFIQ::Image& image, OFIQ::FaceImageQualityAssessment& assessments) = 0;

        /**
         * @brief  This function takes an image and outputs quality information and preprocessing results.
         *
         * @details Implementing functions should be performed on the largest detected face.
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
         * 
         * @param[out] preprocessingResult
         * A container in which the preprocessing results are stored.
         * 
         * @param[in] resultRequestsMask
         * A bit mask encoding the preprocessing result types to be returned.
         *
         * @return OFIQ::ReturnStatus
         * 
         * @see \link OFIQ::PreprocessingResultType PreprocessingResultType\endlink
         */
        virtual OFIQ::ReturnStatus vectorQualityWithPreprocessingResults(
            const OFIQ::Image& image,
            OFIQ::FaceImageQualityAssessment& assessments,
            OFIQ::FaceImageQualityPreprocessingResult& preprocessingResult,
            uint32_t resultRequestsMask) = 0;

        /**
         * @brief
         * Factory method to return a shared pointer to the Interface object.
         * @details
         * This function is implemented by the submitted library and must return
         * a shared pointer to the Interface object.
         *
         * This function MUST be implemented.
         *
         * @note
         * A possible implementation might be:
         * return (std::make_shared<Implementation>());
         * 
         * @return std::shared_ptr<Interface> pointer to the implementation of the interface.
         */
        OFIQ_EXPORT static std::shared_ptr<Interface> getImplementation();

        /**
         * @brief Access version information.
         * @param major Reference to which major version is stored.
         * @param minor Reference to which minor version is stored.
         * @param patch Reference to which patch version is stored.
         */
        OFIQ_EXPORT void getVersion(int& major, int& minor, int& patch) const;

    };
}

#endif /* OFIQ_LIB_H */
