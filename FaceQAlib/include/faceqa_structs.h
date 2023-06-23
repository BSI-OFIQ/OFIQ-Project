#ifndef FACEQA_STRUCTS_H
#define FACEQA_STRUCTS_H

#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace FACEQA
{
    /**
     * @brief
     * Struct representing a single image
     */
    typedef struct Image
    {

        /** Number of pixels horizontally */
        uint16_t width;
        /** Number of pixels vertically */
        uint16_t height;
        /** Number of bits per pixel. Legal values are 8 and 24. */
        uint8_t depth;
        /** Managed pointer to raster scanned data.
         * Either RGB color or intensity.
         * If image_depth == 24 this points to  3WH bytes  RGBRGBRGB...
         * If image_depth ==  8 this points to  WH bytes  IIIIIII */
        std::shared_ptr<uint8_t[]> data;

        Image()
            : width{0},
              height{0},
              depth{24}
        {
        }

        Image(uint16_t width, uint16_t height, uint8_t depth, std::shared_ptr<uint8_t[]>& data)
            : width{width},
              height{height},
              depth{depth},
              data{data}
        {
        }

        /** @brief This function returns the size of the image data. */
        size_t size() const { return (width * height * (depth / 8)); }
    } Image;


    /**
     * @brief
     * Return codes for functions specified in this API
     */
    enum class ReturnCode
    {
        /** Success */
        Success = 0,
        ImageReadingError,
        ImageWritingError,
        /** A required config parameter is missing */
        MissingConfigParamError,
        /** Unable to detect a face in the image */
        FaceDetectionError,
        /** Unable to extract landmarks from face */
        FaceLandmarkExtractionError,
        /** Unable to extract segments from face */
        FaceSegmentationError,
        /** Catch-all error */
        UnknownError,
        /** Failure to generate a quality score on the input image */
        QualityAssessmentError,
        /** Function is not implemented */
        NotImplemented
    };

    /** Output stream operator for a ReturnCode object. */
    inline std::ostream& operator<<(std::ostream& s, const ReturnCode& rc)
    {
        switch (rc)
        {
        case ReturnCode::Success:
            return (s << "Success");
        case ReturnCode::UnknownError:
            return (s << "Unknown Error");
        case ReturnCode::QualityAssessmentError:
            return (s << "Failure to generate a quality score on the input image");
        case ReturnCode::NotImplemented:
            return (s << "Function is not implemented");
        default:
            return (s << "Undefined error");
        }
    }

    /**
     * @brief
     * A structure to contain information about a failure by the software
     * under test.
     *
     * @details
     * An object of this class allows the software to return some information
     * from a function call. The string within this object can be optionally
     * set to provide more information for debugging etc. The status code
     * will be set by the function to Success on success, or one of the
     * other codes on failure.
     */
    typedef struct ReturnStatus
    {
        /** @brief Return status code */
        ReturnCode code;
        /** @brief Optional information string */
        std::string info;

        ReturnStatus()
            : code{ReturnCode::UnknownError},
              info{""}
        {
        }

        /**
         * @brief
         * Create a ReturnStatus object.
         *
         * @param[in] code
         * The return status code; required.
         * @param[in] info
         * The optional information string.
         */
        ReturnStatus(const ReturnCode code, const std::string& info = "")
            : code{code},
              info{info}
        {
        }
    } ReturnStatus;

    typedef struct EyePair
    {
        /** If the left eye coordinates have been computed and
         * assigned successfully, this value should be set to true,
         * otherwise false. */
        bool isLeftAssigned;
        /** If the right eye coordinates have been computed and
         * assigned successfully, this value should be set to true,
         * otherwise false. */
        bool isRightAssigned;
        /** X and Y coordinate of the midpoint between the two canthi of the subject's left eye.  If
         * the eye coordinate is out of range (e.g. x < 0 or x >= width), isLeftAssigned should be
         * set to false, and the left eye coordinates will be ignored. */
        uint16_t xleft;
        uint16_t yleft;
        /** X and Y coordinate of the midpoint between the two canthi of the subject's right eye. If
         * the eye coordinate is out of range (e.g. x < 0 or x >= width), isRightAssigned should be
         * set to false, and the right eye coordinates will be ignored. */
        uint16_t xright;
        uint16_t yright;

        EyePair()
            : isLeftAssigned{false},
              isRightAssigned{false},
              xleft{0},
              yleft{0},
              xright{0},
              yright{0}
        {
        }

        EyePair(
            bool isLeftAssigned,
            bool isRightAssigned,
            uint16_t xleft,
            uint16_t yleft,
            uint16_t xright,
            uint16_t yright)
            : isLeftAssigned{isLeftAssigned},
              isRightAssigned{isRightAssigned},
              xleft{xleft},
              yleft{yleft},
              xright{xright},
              yright{yright}
        {
        }
    } EyePair;

    /** Quality measure labels
     */
    enum class QualityMeasure
    {
        TotalFacesPresent = 0,
        Begin = TotalFacesPresent,
        MouthOpen,
        DynamicRange,
        End
    };

    /** To support iterating over QualityMeasure enum values */
    inline QualityMeasure& operator++(QualityMeasure& qe)
    {
        if (qe == QualityMeasure::End)
            throw std::out_of_range("QualityMeasure& operator++(QualityMeasure&)");
        qe = QualityMeasure(static_cast<std::underlying_type<QualityMeasure>::type>(qe) + 1);
        return qe;
    }

    typedef struct QualityMeasureResult
    {
        /** @brief Raw value as computed by the quality measure implementation */
        double rawScore;
        /** @brief A scalar value from the interval [0,100]
         * Higher values mean higher quality. A value of -1.0 indicates a failed
         * attempt to calculate a quality score or the value is unassigned */
        double scalar;

        QualityMeasureResult()
            : rawScore{-1},
              scalar{-1}
        {
        }

        QualityMeasureResult(double rawScore, double scalar = -1)
            : rawScore{rawScore},
              scalar{scalar}
        {
        }
    } QualityMeasureResult;

    /**
     * @brief
     * Data structure that stores key-value pairs, with each
     * entry representing a quality element and its value
     */
    using QualityAssessments = std::map<QualityMeasure, QualityMeasureResult>;

    enum class FaceDetectorType
    {
        Begin = 0,
        DLIB = Begin,
        OPENCVSSD,
        End,
        NotSet
    };

    typedef struct BoundingBox
    {
        /** @brief leftmost point on head, typically subject's right ear
         *  value must be on [0, imageWidth-1] */
        int16_t xleft = -1;
        /** @brief high point of head, typically top of hair;
         *  value must be on [0, imageHeight-1] */
        int16_t ytop = -1;
        /** @brief bounding box width */
        int16_t width = -1;
        /** @brief bounding box height */
        int16_t height = -1;

        FaceDetectorType faceDetector; // = FaceDetectorType::NotSet;

        float centerX = -1;
        float centerY = -1; 
        float relativeWidth = -1;
        float relativeHeight = -1;
        float rotation = -1;

        BoundingBox()
            : xleft{-1},
              ytop{-1},
              width{-1},
              height{-1}
        {
        }

        BoundingBox(int16_t xleft, int16_t ytop, int16_t width, int16_t height, FaceDetectorType i_faceDetector)
            : xleft{xleft},
              ytop{ytop},
              width{width},
              height{height},
              faceDetector(i_faceDetector)
        {
        }

        BoundingBox(float centerX, float centerY, float relativeWidth, float relativeHeight, float rotation, FaceDetectorType i_faceDetector = FaceDetectorType::NotSet)
            :centerX{centerX},
            centerY{ centerY },
            relativeWidth{ relativeWidth },
            relativeHeight{ relativeHeight },
            rotation{rotation},
            faceDetector(i_faceDetector)
        {
        }

    } BoundingBox;

    typedef struct LandmarkPoint
    {
        int16_t x;
        int16_t y;

        LandmarkPoint()
            : x{-1},
              y{-1}
        {
        }

        LandmarkPoint(int16_t i_x, int16_t i_y)
            : x{i_x},
              y{i_y}
        {
        }
    } LandmarkPoint;

    using Landmarks = std::vector<LandmarkPoint>;

    enum class LandmarkType
    {
        Begin = 0,
        LM_68 = Begin,
        MediaPipe,
        ADNet,
        End,
        NotSet
    };



    typedef struct FaceLandmarks
    {
        LandmarkType type;
        Landmarks landmarks;

        FaceLandmarks()
            : type{LandmarkType::NotSet},
              landmarks{}
        {
        }

    } FaceLandmarks;

    typedef struct FaceImageQualityAssessment
    {

        QualityAssessments qAssessments;
        BoundingBox boundingBox;

        FaceImageQualityAssessment() {}

        FaceImageQualityAssessment(
            const QualityAssessments& qAssessments, const BoundingBox& boundingBox)
            : qAssessments{qAssessments},
              boundingBox{boundingBox}
        {
        }
    } FaceImageQualityAssessment;

}

#endif /* FACEQA_STRUCTS_H */
