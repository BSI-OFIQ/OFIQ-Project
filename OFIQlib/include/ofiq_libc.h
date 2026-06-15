/**
 * @file ofiq_lib.h
 *
 * @copyright Copyright (c) 2025  Federal Office for Information Security, Germany
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
#ifndef OFIQ_LIBC_H
#define OFIQ_LIBC_H

#ifdef _WIN32
#    ifdef OFIQ_EXPORTS
#        define OFIQ_C_EXPORT __declspec(dllexport)
#    else
#        define OFIQ_C_EXPORT __declspec(dllimport)
#    endif
#else
#    define OFIQ_C_EXPORT
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <ofiq_structs_c.h>

    /**
     * @brief Initializes the OFIQ library with the specified configuration file.
     *
     * This function initializes the OFIQ system using the given configuration directory and
     * configuration file name. The handle parameter will be populated with the initialized handle
     * that can be used for subsequent operations.
     *
     * @param[in,out] handle        A pointer to a OFIQHandle where the initialized handle will be
     * stored.
     * @param[in]     configDir     The path to the directory containing the configuration file.
     * @param[in]     configFileName The name of the configuration file to use for initialization.
     *
     * @return ReturnStatus
     *  - ReturnStatus.code == ReturnCode::Success: If the initialization was successful.
     *  - Other error code: Indicates an error occurred during initialization (e.g., invalid path,
     * missing configuration file).
     *
     * @see OFIQCleanup()
     */
    OFIQ_C_EXPORT ReturnStatus
        OFIQInitialize(OFIQHandle* handle, const char* configDir, const char* configFileName);

    /**
     * @brief Cleans up resources initialized by OFIQInitialize().
     *
     * This function cleans up or releases any resources associated with the specified handle. It
     * should be called after initialization to properly terminate and free resources used by the
     * OFIQ library.
     *
     * @param[in] handle The handle obtained from a successful call to ofiqInitialize(). Must not be
     * NULL.
     *
     * @return ReturnStatus
     *  - ReturnStatus.code == ReturnCode::Success: If the cleanup was successful.
     *  - Other error code: Indicates an error occurred during cleanup (e.g., invalid handle).
     *
     * @see OFIQInitialize()
     */
    OFIQ_C_EXPORT ReturnStatus OFIQCleanup(OFIQHandle* handle);

    /**
     * @brief Reads an image from the specified file into the provided Image object.
     *
     * This function loads image data from the given filename and stores it in the provided Image
     * object.
     *
     * @param[in]   filename The path to the image file to read.
     * @param[out]  image The Image object that will be populated with the loaded image data.
     *              This method allocates memory for the Image.data, to free this memory
     *              the method OFIQFreeImage() should be used.
     *
     * @return ReturnStatus
     *  - ReturnStatus.code == ReturnCode::Success: If the image has been read from the file and
     * successfully decoded.
     *  - Other error code: Indicates an error occurred (e.g., file not found, invalid format).
     *
     * @see OFIQFreeImage()
     */
    OFIQ_C_EXPORT ReturnStatus OFIQReadImage(const char* filename, Image& image);

    /**
     * @brief Reads an image from a byte array into the provided Image object.
     *
     * This function reads image data from the specified byte array and populates the provided Image
     * object with the loaded data. The byte array is expected to contain valid image data in a
     * supported format.
     *
     * @param[in]   imageData Pointer to the byte array containing the image data to be read.
     * @param[in]   dataLength Size of the byte array in bytes.
     * @param[out]  image Reference to the Image object that will be populated with the loaded image
     * data. This method allocates memory for the Image.data, to free this memory the method
     * OFIQFreeImage() should be used.
     *
     * @return ReturnStatus
     *  - ReturnStatus.code == ReturnCode::Success: If the image has been read from the byte array
     * and successfully decoded.
     *  - Other error code: Indicates an error occurred during the process, such as invalid data
     * format or insufficient memory.
     *
     * @see OFIQFreeImage()
     * @see ReturnCode
     */
    OFIQ_C_EXPORT ReturnStatus
        OFIQReadImageFromByteArray(const unsigned char* imageData, size_t dataLength, Image& image);

    /**
     * @brief Frees the memory allocated for the specified Image object.
     *
     * This function releases any resources or memory associated with the provided Image object
     * that was previously created by functions like OFIQReadImage().
     *
     * @param image The pointer to the Image object whose memory is to be freed.
     *
     * @return ReturnStatus
     *  - ReturnStatus.code == ReturnCode::Success: If the operation was successful and the image
     * has been properly freed.
     *  - Other error code: Indicates an error occurred, such as an invalid handle or failure to
     * free resources.
     */
    OFIQ_C_EXPORT ReturnStatus OFIQFreeImage(Image* image);

    /**
     * @brief This function computes a scalar image quality measure for the given image.
     *
     * @param[in]   handle  A handle to OFIQ instance.
     * @param[in]   face    Single face image
     * @param[out]  quality A scalar value assessment of image quality. The values are in range
     * [0,100] So, a low value indicates high expected FNMR. A value of -1.0 indicates a failed
     * attempt to calculate a quality score or the value is unassigned.
     *
     * @return ReturnStatus
     *  - ReturnStatus.code == ReturnCode::Success: If the cleanup was successful.
     *  - Other error code: Indicates an error occurred during image analysis.
     */
    OFIQ_C_EXPORT ReturnStatus
        OFIQScalarQuality(OFIQHandle handle, const Image& face, double& quality);

    /**
     * @brief Performs a face image quality assessment using various quality measurement methods
     *
     * @param[in]   handle      A handle to OFIQ instance.
     * @param[in]   faceImage   Single face image
     * @param[out]  qualityAssessment   Results of face image quality assessment containing
     *              a bounding box of the largest face detected and results
     *              of various quality measurements. This method allocates memory
     *              for the FaceImageQualityAssessment object, to free the memory
     *              the method OFIQFreeFaceImageQualityAssessment() should be used.
     *
     * @return ReturnStatus
     *  - ReturnStatus.code == ReturnCode::Success: If the cleanup was successful.
     *  - Other error code: Indicates an error occurred during image analysis.
     *
     * @see FaceImageQualityAssessment
     * @see OFIQFreeFaceImageQualityAssessment()
     */
    OFIQ_C_EXPORT ReturnStatus OFIQVectorQuality(
        OFIQHandle handle, const Image& faceImage, FaceImageQualityAssessment& qualityAssessment);

    /**
     * @brief Performs a face image quality assessment by using various quality measurement methods.
     * Additionally to the #OFIQVectorQuality method this function returns preprocessing results such as
     * - bounding boxes of all faces detected in the processed image
     * - face landmarks of the biggest (most prominent) face
     * - face masks and face segmentation images
     *
     * @param[in]   handle      A handle to OFIQ instance.
     * @param[in]   faceImage   Single face image
     * @param[out]  qualityAssessment   Results of face image quality assessment containing
     *              a bounding box of the largest face detected and results
     *              of various quality measurements. This method allocates memory
     *              for the FaceImageQualityAssessment object, to free the memory
     *              the method OFIQFreeFaceImageQualityAssessment() should be used.
     * @param[out] preprocessingResult  Preprocessing results containing bounding boxes of all faces
     *              detected in the processed image, face landmarks of the most prominent face,
     *              a mask of the face region defined by face landmarks, a face occlusion mask
     *              and a face segmentation image.
     *
     * @return ReturnStatus Indicating whether the operation was successful
     *  - ReturnStatus.code == ReturnCode::Success: If the cleanup was successful.
     *  - Other error code: Indicates an error occurred during image analysis.
     *
     * @see FaceImageQualityAssessment
     * @see PreprocessingResult
     * @see OFIQFreeFaceImageQualityAssessment()
     * @see OFIQFreePreprocessingResult()
     */
    OFIQ_C_EXPORT ReturnStatus OFIQVectorQualityExt(
        OFIQHandle handle,
        const Image& faceImage,
        FaceImageQualityAssessment& qualityAssessment,
        PreprocessingResult& preprocessingResult);

    /**
     * @brief Frees the memory allocated for the specified FaceImageQualityAssessment object.
     *
     * This function releases any resources or memory associated with the provided
     * FaceImageQualityAssessment object that was previously created by function
     * OFIQVectorQuality() or OFIQVectorQualityExt().
     *
     * @param qa The pointer to the FaceImageQualityAssessment object whose memory is to be freed.
     *
     * @return ReturnStatus Indicates whether the operation was successful
     *  - ReturnStatus.code == ReturnCode::Success: If the object has been properly freed.
     *  - Other error code: Indicates an error occurred, such as failure to free resources.
     */
    OFIQ_C_EXPORT ReturnStatus OFIQFreeFaceImageQualityAssessment(FaceImageQualityAssessment* qa);

    /**
     * @brief Frees the memory allocated for the especified FaceLandmarks data.
     *
     * This function releases memory associated with face landmarks, preventing memory leaks.
     * It's essential to ensure that the provided pointer is valid and not null before calling this
     * function.
     *
     * @param landmarks A pointer to the FaceLandmarks object whose memory should be freed.
     *
     * @return ReturnStatus Indicates whether the operation was successful
     *  - ReturnStatus.code == ReturnCode::Success: If the object has been properly freed.
     *  - Other error code: Indicates an error occurred, such as failure to free resources.
     */
    OFIQ_C_EXPORT ReturnStatus OFIQFreeFaceLandmarks(FaceLandmarks* landmarks);

    /**
     * @brief Frees the memory allocated for the specified PreprocessingResult object.
     *
     * This function is used to free the resources associated with a preprocessing result.
     * It ensures that any memory allocated for the result is properly deallocated.
     *
     * @param preprocRes A pointer to the PreprocessingResult object whose resources are to be
     * freed. The caller must ensure this pointer is valid and not NULL to avoid undefined behavior.
     *
     * @return ReturnStatus Indicating whether the operation was successful
     *  - ReturnStatus.code == ReturnCode::Success: If the object has been properly freed.
     *  - Other error code: Indicates an error occurred, such as failure to free resources.
     */
    OFIQ_C_EXPORT ReturnStatus OFIQFreePreprocessingResult(PreprocessingResult* preprocRes);

    /**
     * @brief Retrieves the version information of the OFIQ library.
     *
     * This function provides the version number of the OFIQ library
     * in the form of three integers: major, minor, and patch.
     * These values follow semantic versioning principles, where:
     * - Major version increases with incompatible API changes.
     * - Minor version increases with backward-compatible feature additions.
     * - Patch version increases with backward-compatible bug fixes.
     *
     * @param[out] major The major version number will be stored here.
     * @param[out] minor The minor version number will be stored here.
     * @param[out] patch The patch or build number will be stored here.
     *
     * @example
     * int major, minor, patch;
     * OFIQGetVersion(major, minor, patch);
     * printf("Using OFIQ library version %d.%d.%d\n", major, minor, patch);
     */
    OFIQ_C_EXPORT void OFIQGetVersion(int& major, int& minor, int& patch);

    /**
     * @brief Gets the name of a quality measure as a string.
     *
     * This function returns the name of the specified quality measure as a null-terminated string.
     * The returned string should not be modified or freed by the caller.
     *
     * @param measure The QualityMeasure value for which to get the name.
     * @return A pointer to a const char array containing the name of the quality measure.
     */
    OFIQ_C_EXPORT const char* OFIQGetQualityMeasureName(QualityMeasure measure);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* OFIQ_LIBC_H */
