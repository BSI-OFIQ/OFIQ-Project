/**
 * @file ofiq_structs_c.h
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
 * @brief Provides several helper classes, enums and functions used in the OFIQ framework.
 * @author OFIQ development team
 */
#ifndef OFIQ_STRUCTS_C_H
#define OFIQ_STRUCTS_C_H

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
using ReturnStatus = ReturnCode;
//struct ReturnStatus
//{
//    /** @brief Return status code */
//    ReturnCode code{ReturnCode::UnknownError};
//    /** @brief Optional information string */
//    char info[STATUS_INFO_LENGHT]{};
//};

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
#endif /* OFIQ_STRUCTS_C_H */
