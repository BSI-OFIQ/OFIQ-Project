
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

#include <stdint.h>
#include <stddef.h>

/**
 * \brief Handle type for an OFIQ implementation instance
 */
using OFIQHandle = void*;

/**
 * @brief
 * Return codes for functions specified in this API
 */
enum class ReturnCode
{
    /** Success */
    Success = 0,
    /** Failed to read an image. */
    ImageReadingError,
    /** failed to write an image to disk. */
    ImageWritingError,
    /** A config file is missing or the given filename is invalid*/
    MissingConfigFileError,
    /** A required config parameter is missing */
    MissingConfigParamError,
    /** A required config parameter is missing */
    UnknownConfigParamError,
    /** Unable to detect a face in the image */
    FaceDetectionError,
    /** Unable to extract landmarks from face */
    FaceLandmarkExtractionError,
    /** Unable to extract occlusion segments from face */
    FaceOcclusionSegmentationError,
    /** Unable to parse face */
    FaceParsingError,
    /** Catch-all error */
    UnknownError,
    /** Failure to generate a quality score on the input image */
    QualityAssessmentError,
    /** Function is not implemented */
    NotImplemented
};

constexpr size_t STATUS_INFO_LENGHT = 512;

/**
 * @brief
 * A structure containing return status - a code and a information message
 * under test.
 *
 * @details
 * An object of this class allows the software to return some information
 * from a function call. The string within this object can be optionally
 * set to provide more information for debugging etc. The status code
 * will be set by the function to Success on success, or one of the
 * other codes on failure.
 */
struct ReturnStatus
{
    /** @brief Return status code */
    ReturnCode code{ReturnCode::UnknownError};
    /** @brief Optional information string */
    char info[STATUS_INFO_LENGHT]{};
};

/**
 * @brief
 * Struct representing a single image
 */
struct Image
{
    /** Number of pixels horizontally */
    uint16_t width{0};
    /** Number of pixels vertically */
    uint16_t height{0};
    /** Number of bits per pixel. Legal values are 8 and 24. */
    uint8_t depth{24};
    /** Managed pointer to pixes data.
     * Either BGR color or intensity.
     * If image_depth == 24 this points to  3WH bytes  BGRBGRBGR...
     * If image_depth ==  8 this points to  WH bytes  IIIIIII */
    uint8_t* data{nullptr};
};

/** @brief Enumeration of the measure labels
 */
enum class QualityMeasure
{
    /** UnifiedQualityScore*/
    UnifiedQualityScore = 0x41,
    /** BackgroundUniformity*/
    BackgroundUniformity = 0x42,
    /** IlluminationUniformity*/
    IlluminationUniformity = 0x43,
    /** the common measure implementation for LuminanceMean, LuminanceVariance */
    Luminance = -0x44,
    /** LuminanceMean*/
    LuminanceMean = 0x44,
    /** LuminanceVariance*/
    LuminanceVariance = 0x45,
    /** UnderExposurePrevention*/
    UnderExposurePrevention = 0x46,
    /** OverExposurePrevention*/
    OverExposurePrevention = 0x47,
    /** DynamicRange*/
    DynamicRange = 0x48,
    /** Sharpness*/
    Sharpness = 0x49,
    /** CompressionArtifacts*/
    CompressionArtifacts = 0x4a,
    /** NaturalColour*/
    NaturalColour = 0x4b,
    /** SingleFacePresent */
    SingleFacePresent = 0x4c,
    /** EyesOpen*/
    EyesOpen = 0x4d,
    /** MouthClosed*/
    MouthClosed = 0x4e,
    /** EyesVisible*/
    EyesVisible = 0x4f,
    /** MouthOcclusionPrevention*/
    MouthOcclusionPrevention = 0x50,
    /** FaceOcclusionPrevention*/
    FaceOcclusionPrevention = 0x51,
    /** InterEyeDistance*/
    InterEyeDistance = 0x52,
    /** HeadSize*/
    HeadSize = 0x53,
    /** CropOfTheFaceImage: common measure for {Left,Right}wardCropOfTheFaceImage, MarginAbove, and
       MarginBelow */
    CropOfTheFaceImage = -0x54,
    /** LeftwardCropOfTheFaceImage*/
    LeftwardCropOfTheFaceImage = 0x54,
    /** RightwardCropOfTheFaceImage*/
    RightwardCropOfTheFaceImage = 0x55,
    /** MarginAbove */
    MarginAboveOfTheFaceImage = 0x56,
    /** MarginBelow */
    MarginBelowOfTheFaceImage = 0x57,
    /** HeadPose*/
    HeadPose = -0x58,
    /** HeadPoseYaw*/
    HeadPoseYaw = 0x58,
    /** HeadPosePitch*/
    HeadPosePitch = 0x59,
    /** HeadPoseRoll*/
    HeadPoseRoll = 0x5a,
    /** ExpressionNeutrality*/
    ExpressionNeutrality = 0x5b,
    /** NoHeadCoverings */
    NoHeadCoverings = 0x5c,
    /** unknown measure*/
    NotSet = -1
};

/**
 * @brief
 * Return codes for QualityMeasureResult
 */
enum class QualityMeasureReturnCode
{
    /** Success */
    Success = 0,
    /** Unable to assess a quality measure */
    FailureToAssess,
    /** Quality measure is not initialized */
    NotInitialized
};

/**
 * @brief Data structure to handle the results of a quality measure.
 *
 */
struct QualityMeasureResult
{
    QualityMeasure measure{QualityMeasure::NotSet};

    /** @brief Raw value as computed by the quality measure implementation */
    double rawScore{-1};
    /** @brief A scalar value from the interval [0,100]
     * Higher values mean higher quality. A value of -1.0 indicates a failed
     * attempt to calculate a quality score or the value is unassigned */
    double scalar{-1};
    /** @brief Return status code */
    QualityMeasureReturnCode code{QualityMeasureReturnCode::NotInitialized};
};


/**
 * @brief Enum containing different face detector types
 *
 */
enum class FaceDetectorType
{
    /** face detector based on the ssd implementation in opencv.*/
    OPENCVSSD,
    /** unknown face detector*/
    NotSet
};


/**
 * @brief Data structure for descibing bounding boxes,
 * e.g. the face region of the faces found by a face detector.
 *
 */
struct BoundingBox
{
    /** @brief leftmost point on head, typically subject's right ear
     *  value must be on [0, imageWidth-1] */
    int16_t xleft{-1};
    /** @brief high point of head, typically top of hair;
     *  value must be on [0, imageHeight-1] */
    int16_t ytop{-1};
    /** @brief bounding box width */
    int16_t width{-1};
    /** @brief bounding box height */
    int16_t height{-1};

    /** @brief Description of the face detector used. */
    FaceDetectorType faceDetector = FaceDetectorType::NotSet;
};

/**
 * @brief Data structure to describe the x and y coordinate of a landmark.
 *
 */
struct LandmarkPoint
{
    /**
     * @brief x - coordinate
     *
     */
    int16_t x{-1};
    /**
     * @brief y - coordinate
     *
     */
    int16_t y{-1};
};

/**
 * @brief container for a collection of landmarks, e.g. belonging to all the landmarks detected on a
 * face image.
 *
 */
// using Landmarks = LandmarkPoint*;

/**
 * @brief Enum describing the different implementations of landmarks.
 *
 */
enum class LandmarkType
{
    /** Landmarks extracted with the adnet detector.*/
    LM_98,
    /** used for iterating through the enums.*/
    NotSet
};


/**
 * @brief Data structure for storing facial landmarks
 *
 */
struct FaceLandmarks
{
    /** Enum describing the type of the landmarks.*/
    LandmarkType type{LandmarkType::NotSet};
    /** container for all detected landmarks. */
    LandmarkPoint* landmarks;
    /**
     * @brief Number of landmark points stored in #landmarks
     */
    int numLandmarks = 0;
};

/**
 * @brief Data structure storing the results of the different measurement computations.
 *
 */
struct FaceImageQualityAssessment
{
    /**
     * @brief Container for storing the resuls of the different measure computations.
     */
    QualityMeasureResult* qAssessments{nullptr};

    /**
     * @brief Number of quality measure computations stored in #qAssessments
     */
    size_t numAssessments{0};

    /**
     * @brief Face region described by bounding box.
     */
    BoundingBox boundingBox;
};

struct PreprocessingResult
{
    /**
     * @brief Bounding boxes of detected faces
     */
    BoundingBox* faces;

    /**
     * @brief Number of detected faces
     */
    size_t numFaces;

    /**
     * @brief Facial landmarks
     */
    FaceLandmarks landmarks;

    /**
     * @brief Segmentation mask
     * @details For each pixel at index (y * width + x) of an image of dimension (height,width),
     * the value is assigned a segment type from the following table.
     * <table>
     *  <tr><td><b>code</b></td><td><b>segment type</b></td></tr>
     *  <tr><td>0</td><td>background</td></tr>
     *  <tr><td>1</td><td>face skin</td></tr>
     *  <tr><td>2</td><td>left eye brow</td></tr>
     *  <tr><td>3</td><td>right eye brow</td></tr>
     *  <tr><td>4</td><td>left eye</td></tr>
     *  <tr><td>5</td><td>right eye</td></tr>
     *  <tr><td>6</td><td>eyeglasses</td></tr>
     *  <tr><td>7</td><td>left ear</td></tr>
     *  <tr><td>8</td><td>right ear</td></tr>
     *  <tr><td>9</td><td>earring</td></tr>
     *  <tr><td>10</td><td>nose</td></tr>
     *  <tr><td>11</td><td>mouth</td></tr>
     *  <tr><td>12</td><td>upper lip</td></tr>
     *  <tr><td>13</td><td>lower lip</td></tr>
     *  <tr><td>14</td><td>neck</td></tr>
     *  <tr><td>15</td><td>necklace</td></tr>
     *  <tr><td>16</td><td>clothing</td></tr>
     *  <tr><td>17</td><td>hair</td></tr>
     *  <tr><td>18</td><td>head covering</td></tr>
     *  <tr><td>19</td><td>undocumented</td></tr>
     *  <tr><td>20</td><td>undocumented</td></tr>
     *  <tr><td>21</td><td>undocumented</td></tr>
     *  <tr><td>22</td><td>undocumented</td></tr>
     *  <tr><td>23</td><td>undocumented</td></tr>
     *  <tr><td>255</td><td>outside of the aligned face image</td></tr>
     * </table>
     */
    Image segmentationMask;

    /**
     * @brief Occlusion mask
     * @details For each pixel at index (y * width + x) of an image of dimension (height,width),
     * the value is 0 if the pixel is not part of the occlusion mask; otherwise, the value is 1.
     */
    Image occlusionMask;

    /**
     * @brief Landmarked region
     * @details For each pixel at index (y * width + x) of an image of dimension (height,width),
     * the value is 0 if the pixel is not part of the landmarked region mask; otherwise, the value is 1
     */
    Image landmarkedRegionMask;

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
     *  - ReturnStatus.code == #ReturnCode::Success: If the image has been read from the byte array
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
     * @brief Performs a face image quality assessment by using various quality measurement methods
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
