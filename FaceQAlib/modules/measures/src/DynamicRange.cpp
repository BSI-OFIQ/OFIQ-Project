#include "DynamicRange.h"
#include "FaceMeasures.h"
#include "utils.h"
#include <opencv2/imgproc.hpp>

namespace FACEQA_LIB::modules::measures
{
    static const auto qualityMeasure = FACEQA::QualityMeasure::DynamicRange;

    static double ComputeEntropy(const cv::Mat& luminanceImage, const cv::Mat& maskImage);
    static double CalculateScore(const cv::Mat1f& histogram);

    DynamicRange::DynamicRange(
        const Configuration& configuration,
        Session& session,
        FaceLandmarkExtractorInterface& landmarkExtractor)
        : Measure{configuration, session},
          landmarkExtractor{landmarkExtractor}
    {
        SigmoidParameters defaultValues;
        defaultValues.x0 = 5;
        defaultValues.w = 1;
        AddSigmoid(qualityMeasure, defaultValues);
    }

    void DynamicRange::Execute()
    {
        auto mask = landmarks::FaceMeasures::GetFaceMask(
            landmarkExtractor.extractLandmarks(session),
            session.image);
        auto cvMask = copyToCvImage(mask, true);
        auto cvImage = copyToCvImage(session.image);
        cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2YUV);

        auto rawScore = ComputeEntropy(cvImage, cvMask);
        SetQualityMeasure(qualityMeasure, rawScore);
    }

    static double ComputeEntropy(const cv::Mat& luminanceImage, const cv::Mat& maskImage)
    {
        int histSize = 256;
        std::vector<float> range = {0, 256};
        cv::Mat1f histogram;

        cv::calcHist(std::vector{luminanceImage}, {0}, maskImage, histogram, {histSize}, range);

        return CalculateScore(histogram);
    }

    static double CalculateScore(const cv::Mat1f& histogram)
    {
        auto pixelsInHistogram = cv::sum(histogram).val[0];
        double rawScore = 0;

        for (auto item : histogram)
        {
            auto normalizedValue = item / pixelsInHistogram;
            if (normalizedValue != 0)
            {
                rawScore += normalizedValue * log2(normalizedValue);
            }
        }

        rawScore *= -1;
        return rawScore;
    }
}