#include "ofiq_libc.h"
#include "ofiq_structs_c.h"
#include "ofiq_libc_utils.h"

#include "ofiq_lib.h"
#include "image_io.h"

#include <sstream>


OFIQ_C_EXPORT void OFIQGetVersion(int& major, int& minor, int& patch)
{
    major = int(OFIQ_VERSION_MAJOR);
    minor = int(OFIQ_VERSION_MINOR);
    patch = int(OFIQ_VERSION_PATCH);
    return;
}

OFIQ_C_EXPORT ReturnStatus OFIQInitialize(
    OFIQHandle* handle,
    const char* configDir,
    const char* configFileName)
{
    // yes, this is a pointer to a shared_ptr
    auto implPtr = new std::shared_ptr<OFIQ::Interface>(OFIQ::Interface::getImplementation());
    
    if (auto ret = (*implPtr)->initialize(
            std::string(configDir),
            std::string(configFileName)); 
        ret.code != OFIQ::ReturnCode::Success)
    {
        std::stringstream sstr;
        sstr << "Failed to initialize OFIQ instance from config directory "
            << configDir << " and config file " << configFileName;
        return createReturnStatus(static_cast<ReturnStatus>(ret.code), sstr.str());
    }
    
    *handle = static_cast<OFIQHandle>(implPtr);
    
    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT ReturnStatus OFIQCleanup(
    OFIQHandle* handle)
{
    if (handle == nullptr)
        return createReturnStatus(
            ReturnCode::UnknownError, 
            "The given handle is a nullptr");

    auto implPtr = static_cast<std::shared_ptr<OFIQ::Interface>*>(*handle);
    if (!implPtr)
    {
        return createReturnStatus(
            ReturnCode::UnknownError,
            "The given handle is an invalid instance");
    }

    implPtr->reset();
    delete implPtr;
    *handle = nullptr;

    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT ReturnStatus OFIQReadImage(
    const char* filename,
    Image& image)
{
    OFIQ::Image ofiqImage;
    if (OFIQ::ReturnStatus ofiqRetStat = OFIQ_LIB::readImage(
        std::string(filename), ofiqImage);
        ofiqRetStat.code != OFIQ::ReturnCode::Success)
    {
        return createReturnStatus(ReturnCode::ImageReadingError);
    }

    if (ReturnStatus retStat = copyImageToCApi(ofiqImage, image);
        retStat/*.code*/ != ReturnCode::Success)
    {
        return retStat;
    }

    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT ReturnStatus OFIQReadImageFromByteArray(
    const unsigned char* imageData,
    size_t dataLength,
    Image& image)
{
    OFIQ::Image ofiqImage;
    std::vector<unsigned char> imgDataVector(imageData, imageData+dataLength);
    if (OFIQ::ReturnStatus ofiqRetStat = OFIQ_LIB::readImageFromByteArray(
        imgDataVector, ofiqImage);
        ofiqRetStat.code != OFIQ::ReturnCode::Success)
    {
        return createReturnStatus(ReturnCode::ImageReadingError);
    }

    if (ReturnStatus retStat = copyImageToCApi(ofiqImage, image);
        retStat/*.code */!= ReturnCode::Success)
    {
        return retStat;
    }

    return createReturnStatus(ReturnCode::Success);
}


OFIQ_C_EXPORT ReturnStatus OFIQFreeImage(Image* image)
{
    if (image->data)
        delete[]image->data;
    image->data = nullptr;
    image->width = image->height = image->depth = 0;
    return createReturnStatus(ReturnCode::Success);
}


OFIQ_C_EXPORT ReturnStatus OFIQScalarQuality(
    OFIQHandle handle,
    const Image& face,
    double& quality)
{
    auto implPtr = static_cast<std::shared_ptr<OFIQ::Interface>*>(handle);

    OFIQ::Image ofiqImage;
    copyImageFromCApi(face, ofiqImage);

    OFIQ::ReturnStatus ofiqRetStat = (*implPtr)->scalarQuality(
        ofiqImage, quality);

    if (ofiqRetStat.code != OFIQ::ReturnCode::Success)
    {
        return createReturnStatus(ReturnCode::QualityAssessmentError,
            "Failed to perform scalar quality computation");
    }

    return createReturnStatus(ReturnCode::Success);
}


OFIQ_C_EXPORT ReturnStatus OFIQVectorQuality(
    OFIQHandle handle,
    const Image& faceImage,
    FaceImageQualityAssessment& qualityAssessment)
{
    auto implPtr = static_cast<std::shared_ptr<OFIQ::Interface>*>(handle);

    OFIQ::Image ofiqImage;
    copyImageFromCApi(faceImage, ofiqImage);

    OFIQ::FaceImageQualityAssessment ofiqAssessment;
    OFIQ::ReturnStatus ofiqRetStat = (*implPtr)->vectorQuality(
        ofiqImage, ofiqAssessment);

    if (ofiqRetStat.code != OFIQ::ReturnCode::Success)
    {
        return createReturnStatus(ReturnCode::QualityAssessmentError,
            "Failed to perform vector quality assessment");
    }

    if (ReturnStatus retStat = copyQualityAssessmentToCApi(
        ofiqAssessment, qualityAssessment);
        retStat/*.code*/ != ReturnCode::Success)
    {
        return retStat;
    }

    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT ReturnStatus OFIQVectorQualityExt(
    OFIQHandle handle,
    const Image& faceImage,
    FaceImageQualityAssessment& qualityAssessment,
    PreprocessingResult& preprocessingResult)
{
    auto implPtr = static_cast<std::shared_ptr<OFIQ::Interface>*>(handle);

    OFIQ::Image ofiqImage;
    copyImageFromCApi(faceImage, ofiqImage);

    OFIQ::FaceImageQualityAssessment ofiqAssessment;
    OFIQ::FaceImageQualityPreprocessingResult ofiqPreprocResult;
    OFIQ::ReturnStatus ofiqRetStat = (*implPtr)->vectorQualityWithPreprocessingResults(
        ofiqImage, ofiqAssessment, ofiqPreprocResult,
        static_cast<uint32_t>(OFIQ::PreprocessingResultType::All));

    if (ofiqRetStat.code != OFIQ::ReturnCode::Success)
    {
        return createReturnStatus(ReturnCode::QualityAssessmentError,
            "Failed to perform vector quality assessment");
    }

    if (ReturnStatus retStat = copyQualityAssessmentToCApi(
        ofiqAssessment, qualityAssessment);
        retStat/*.code*/ != ReturnCode::Success)
    {
        return retStat;
    }

    if (ReturnStatus retStat = copyPreprocessingResultToCApi(
            ofiqPreprocResult,
            ofiqImage.width,
            ofiqImage.height,
            preprocessingResult);
        retStat/*.code*/ != ReturnCode::Success)
    {
        return retStat;
    }

    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT ReturnStatus OFIQFreeFaceImageQualityAssessment(
    FaceImageQualityAssessment* qa)
{
    if (qa == nullptr)
        return createReturnStatus( ReturnCode::UnknownError,
            "I refuse to free a nullptr to FaceImageQualityAssessment");
        
    if (qa->qAssessments)
        delete[]qa->qAssessments;
    qa->qAssessments = nullptr;
    qa->numAssessments = 0;
    qa->boundingBox = BoundingBox();
    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT ReturnStatus OFIQFreeFaceLandmarks(
    FaceLandmarks* faceLandmarks)
{
    if (faceLandmarks == nullptr)
        return createReturnStatus(ReturnCode::UnknownError,
            "I refuse to free a nullptr to FaceLandmarks");
    if (faceLandmarks->landmarks)
        delete[]faceLandmarks->landmarks;
    faceLandmarks->landmarks = nullptr;
    faceLandmarks->numLandmarks = 0;
    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT ReturnStatus OFIQFreePreprocessingResult(
    PreprocessingResult* preprocRes)
{
    if (preprocRes == nullptr)
        return createReturnStatus(ReturnCode::UnknownError, 
            "I refuse to free a nullptr to PreprocessingResult");

    // free mask images
    OFIQFreeImage(&(preprocRes->segmentationMask));
    OFIQFreeImage(&(preprocRes->occlusionMask));
    OFIQFreeImage(&(preprocRes->landmarkedRegionMask));

    // free landmarks
    OFIQFreeFaceLandmarks(&(preprocRes->landmarks));

    // free face bounding boxes
    if (preprocRes->faces)
        delete[]preprocRes->faces;
    preprocRes->faces = nullptr;
    preprocRes->numFaces = 0;

    return createReturnStatus(ReturnCode::Success);
}

OFIQ_C_EXPORT const char* OFIQGetQualityMeasureName(
    QualityMeasure measure)
{
    return getQualityMeasureName(measure);
}