#include "ofiq_libc_utils.h"
#include <magic_enum.hpp>
#include <memory>
#include <map>

std::map<QualityMeasure, std::string> generateQualityMeasureNames();

static const std::map<QualityMeasure, std::string> QualityMeasureNames = generateQualityMeasureNames();

std::map<QualityMeasure, std::string> generateQualityMeasureNames()
{
    std::map<QualityMeasure, std::string> measureNames;
    for (const auto& enumValue : magic_enum::enum_values<QualityMeasure>())
        measureNames.emplace(enumValue, OFIQ::QualityMeasureToString(
            static_cast<OFIQ::QualityMeasure>((int)enumValue)));
    return measureNames;
}

const char* getQualityMeasureName(QualityMeasure measure)
{
    static const char* notFoundName = "UnknownQualityMeasure";
    const auto iter = QualityMeasureNames.find(measure);
    if (iter == QualityMeasureNames.cend())
        return notFoundName;
    else
        return iter->second.c_str();
}

ReturnStatus createReturnStatus(ReturnCode code, std::string_view str)
{
//    ReturnStatus ret{code, ""};
//#if defined(_WINDOWS)
//    strcpy_s(ret.info, sizeof ret.info, std::string(str).c_str());
//#else
//    strncpy(ret.info, std::string(str).c_str(), sizeof ret.info);
//#endif
//    return ret;
    return code;
}

ReturnStatus copyImageToCApi(
    const OFIQ::Image& ofiqImage,
    Image& capiImage)
{
    size_t imageDataSize = ofiqImage.size();
    bool memoryAllocFailed = false;
    try
    {
        capiImage.data = new uint8_t[imageDataSize];
    }
    catch (const std::bad_alloc&)
    {
        memoryAllocFailed = true;
    }

    if (capiImage.data == nullptr)
        memoryAllocFailed = true;

    if (memoryAllocFailed)
    {
        return createReturnStatus(
            ReturnCode::UnknownError,
            "Memory allocation failed for image data");
    }

    capiImage.width = ofiqImage.width;
    capiImage.height = ofiqImage.height;
    capiImage.depth = ofiqImage.depth;

    memcpy(capiImage.data, ofiqImage.data.get(), imageDataSize);
    
    return createReturnStatus(ReturnCode::Success);
}

ReturnStatus copyImageFromCApi(
    const Image& capiImage,
    OFIQ::Image& ofiqImage)
{
    ofiqImage.width = capiImage.width;
    ofiqImage.height = capiImage.height;
    ofiqImage.depth = capiImage.depth;
    size_t size = ofiqImage.size();
    ofiqImage.data.reset(new uint8_t[size], std::default_delete<uint8_t[]>());
    memcpy(ofiqImage.data.get(), capiImage.data, size);

    return createReturnStatus(ReturnCode::Success);
}

ReturnStatus copyQualityAssessmentToCApi(
    const OFIQ::FaceImageQualityAssessment& ofiqQA,
    FaceImageQualityAssessment& qa)
{
    // copy bounding box values
    qa.boundingBox.xleft = ofiqQA.boundingBox.xleft;
    qa.boundingBox.ytop = ofiqQA.boundingBox.ytop;
    qa.boundingBox.width = ofiqQA.boundingBox.width;
    qa.boundingBox.height = ofiqQA.boundingBox.height;

    if ((int)ofiqQA.boundingBox.faceDetector == (int)FaceDetectorType::OPENCVSSD)
        qa.boundingBox.faceDetector = FaceDetectorType::OPENCVSSD;
    else
        qa.boundingBox.faceDetector = FaceDetectorType::NotSet;

    //copy assessment results
    qa.numAssessments = ofiqQA.qAssessments.size();
    qa.qAssessments = new QualityMeasureResult[qa.numAssessments];
    int i = 0;
    for (auto const& [measure, result] : ofiqQA.qAssessments)
    {
        qa.qAssessments[i].measure = QualityMeasureToCApi(measure);
        qa.qAssessments[i].code = QualityMeasureReturnCodeToCApi(result.code);
        qa.qAssessments[i].rawScore = result.rawScore;
        qa.qAssessments[i].scalar = result.scalar;
        i++;
    }

    return createReturnStatus(ReturnCode::Success);
}

ReturnStatus copyPreprocessingResultToCApi(
    const OFIQ::FaceImageQualityPreprocessingResult& ofiqPreprocRes,
    const uint16_t imageWidth,
    const uint16_t imageHeight,
    PreprocessingResult& preprocRes)
{
    // copy face detection results
    preprocRes.numFaces = ofiqPreprocRes.m_faces.size();
    preprocRes.faces = new BoundingBox[preprocRes.numFaces];
    for (size_t i = 0; i < preprocRes.numFaces; i++)
    {
        preprocRes.faces[i].xleft = ofiqPreprocRes.m_faces[i].xleft;
        preprocRes.faces[i].ytop = ofiqPreprocRes.m_faces[i].ytop;
        preprocRes.faces[i].width = ofiqPreprocRes.m_faces[i].width;
        preprocRes.faces[i].height = ofiqPreprocRes.m_faces[i].height;
        preprocRes.faces[i].faceDetector =
            ofiqPreprocRes.m_faces[i].faceDetector == OFIQ::FaceDetectorType::OPENCVSSD
                ? FaceDetectorType::OPENCVSSD
                : FaceDetectorType::NotSet;
    }

    // copy landmarks
    preprocRes.landmarks.numLandmarks = ofiqPreprocRes.m_landmarks.landmarks.size();
    preprocRes.landmarks.landmarks = new LandmarkPoint[preprocRes.landmarks.numLandmarks];
    for (size_t i = 0; i < preprocRes.landmarks.numLandmarks; i++)
    {
        preprocRes.landmarks.landmarks[i].x = ofiqPreprocRes.m_landmarks.landmarks[i].x;
        preprocRes.landmarks.landmarks[i].y = ofiqPreprocRes.m_landmarks.landmarks[i].y;
    }
    preprocRes.landmarks.type = ofiqPreprocRes.m_landmarks.type == OFIQ::LandmarkType::LM_98
        ? LandmarkType::LM_98
        : LandmarkType::NotSet;

    // copy mask images
    constexpr uint8_t maskImgDepth = 8;

    copyImageToCApi(
        OFIQ::Image(imageWidth, imageHeight, maskImgDepth, ofiqPreprocRes.m_landmarkedRegionPtr),
        preprocRes.landmarkedRegionMask);
    
    copyImageToCApi(
        OFIQ::Image(imageWidth, imageHeight, maskImgDepth, ofiqPreprocRes.m_occlusionMaskPtr),
        preprocRes.occlusionMask);

    copyImageToCApi(
        OFIQ::Image(imageWidth, imageHeight, maskImgDepth, ofiqPreprocRes.m_segmentationMaskPtr),
        preprocRes.segmentationMask);

    return createReturnStatus(ReturnCode::Success);
}

QualityMeasure QualityMeasureToCApi(
    OFIQ::QualityMeasure measure)
{
    QualityMeasure qm;
    try
    {
        qm = static_cast<QualityMeasure>((int)measure);
    }
    catch (const std::exception&)
    {
        qm = QualityMeasure::NotSet;
    }
    return qm;
}

QualityMeasureReturnCode QualityMeasureReturnCodeToCApi(
    OFIQ::QualityMeasureReturnCode qmCode)
{
    QualityMeasureReturnCode code;
    try
    {
        code = static_cast<QualityMeasureReturnCode>((int)qmCode);
    }
    catch (const std::exception&)
    {
        code = QualityMeasureReturnCode::NotInitialized;
    }
    return code;
}