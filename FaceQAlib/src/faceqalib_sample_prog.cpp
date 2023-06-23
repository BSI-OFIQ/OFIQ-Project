
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cmath>
//#include <csignal>

#define SUCCESS 0
#define FAILURE 1
#define NOT_IMPLEMENTED 2

#if defined _WIN32 && defined FACEQA_EXPORTS
#    undef FACEQA_EXPORTS
//#define FACEQA_EXPORT  __declspec(dllimport)
#endif

#include "faceqa_lib.h"
#include "image_io.h"
#include "utils.h"
#include <magic_enum.hpp>
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;
using namespace FACEQA;
using namespace FACEQA_LIB;


int getQualityAssessmentResults(
    std::shared_ptr<Interface>& implPtr,
    const string& inputFile,
    FaceImageQualityAssessment& assessments);

string exportAssessmentResultsToString(
    const FaceImageQualityAssessment& assessments,
    bool doExportScalar = false);

bool isStringContained(
    const std::vector<std::string>& strings, 
    const std::string& str,
    bool isCaseSensitive=false)
{
    std::string s = str;
    if(!isCaseSensitive)
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); });

    return (!s.empty() &&
        std::find(strings.begin(), strings.end(), s) != strings.end());
}

int runQuality(
    std::shared_ptr<Interface>& implPtr, const string& inputFile, const std::optional<string> & outputFile)
{
    fs::path inputFilePath(inputFile);
    std::string fileExt = inputFilePath.extension().string();
  
    FaceImageQualityAssessment assessment;
    int resultCode = getQualityAssessmentResults(implPtr, inputFile, assessment);

    string rawStr = exportAssessmentResultsToString(assessment, false);
    string scalarStr = exportAssessmentResultsToString(assessment, true);

    if (outputFile.has_value())
    {
        std::ofstream ofs(outputFile.value());
        if (ofs.good())
        {
            ofs << "ImageFile";
            for (auto const& aResult : assessment.qAssessments )
            {
                string mName = static_cast<std::string>(magic_enum::enum_name(aResult.first));
                ofs << ';' << mName << ".raw" << ';' << mName << ".scalar";
            }
            ofs << endl;

            ofs << inputFile;
            for (auto const& aResult : assessment.qAssessments)
            {
                ofs << ';' << aResult.second.rawScore << ';' << aResult.second.scalar;
            }
        }
    }
        
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << "ImagFile: '" << inputFile << "' has attributes:" << std::endl; 
    for (auto const& aResult : assessment.qAssessments)
    {
        string mName = static_cast<std::string>(magic_enum::enum_name(aResult.first));
        auto rawScore = aResult.second.rawScore;
        auto scalarScore = aResult.second.scalar;
        std::cout << mName << "-> rawScore:  " << rawScore << "   scalar: " << scalarScore << std::endl;
    }
    std::cout << "-------------------------------------------------------" << std::endl;

    return SUCCESS;
}

int getQualityAssessmentResults(
    std::shared_ptr<Interface>& implPtr,
    const string& inputFile,
    FaceImageQualityAssessment& assessments)
{
    Image image;
    ReturnStatus retStatus = readImage(inputFile, image);

    if (retStatus.code != ReturnCode::Success)
    {
        cerr << "[ERROR] " << retStatus.info << "." << endl;
        return FAILURE;
    }

    retStatus = implPtr->vectorQuality(image, assessments);

    double quality;
    ReturnStatus retStatus2 = implPtr->scalarQuality(image, quality);
    std::cout << "Image file: '" << inputFile << "' scalar quality: " << quality << std::endl; 
     
    return retStatus.code == ReturnCode::Success ? SUCCESS : FAILURE;
}

string exportAssessmentResultsToString(
    const FaceImageQualityAssessment& assessments, 
    bool doExportScalar)
{
    std::string resultStr;
    char buf[200] = {};
    for (auto const& aResult : assessments.qAssessments)
    {
        //const QualityMeasure& measure = aResult.first;
        const QualityMeasureResult& qaResult = aResult.second;
        double val = doExportScalar ? qaResult.scalar : qaResult.rawScore;
        if(round(val) == val)
            snprintf(buf, 200, "%d;", (int)val);
        else
            snprintf(buf, 200, "%f;", val);
        resultStr += std::string(buf);
    }
    return resultStr;
}


void usage(const string& executable)
{
    cerr << "Usage: " << executable
         << " -c configDir [-cf configFile] "
            "-o outputFile -i inputFile"
         << endl;
    exit(EXIT_FAILURE);
}

// command arguments that can be used for debugging in VisualStudio:
// -c ..\..\data\ -i ..\..\data\images\c-07-twofaces.png

// command arguments that can be used for debugging in VisualStudio including a custom config file:
// -c ..\..\data\ -cf faceqa.jaxn -i ..\..\data\images\c-07-twofaces.png 

int main(int argc, char* argv[])
{
    int requiredArgs = 1; /* exec name */
    if (argc < requiredArgs)
        usage(argv[0]);

    std::optional<string> configDir;
    std::optional<string> outputFile;
    std::optional<string> inputFile;
    std::optional<string> configFile;

    for (int i = 0; i < argc - requiredArgs; i++)
    {
        if (strcmp(argv[requiredArgs + i], "-c") == 0)
            configDir = argv[requiredArgs + (++i)];
        else if (strcmp(argv[requiredArgs + i], "-o") == 0)
            outputFile = argv[requiredArgs + (++i)];
        else if (strcmp(argv[requiredArgs + i], "-i") == 0)
            inputFile = argv[requiredArgs + (++i)];
        else if (strcmp(argv[requiredArgs + i], "-cf") == 0)
            configFile = argv[requiredArgs + (++i)];
        else
        {
            cerr << "[ERROR] Unrecognized flag: " << argv[requiredArgs + i] << endl;
            usage(argv[0]);
            exit(-1);
        }
    }

    if (!configDir.has_value())
    {
        cerr << "[ERROR] configDir not specified." << endl;
        usage(argv[0]);
        exit(-1);
    }

    if (!configFile.has_value())
    {
        configFile = "faceqa.jaxn";
    }

    if (!inputFile.has_value())
    {
        cerr << "[ERROR] inputFile not specified." << endl;
        usage(argv[0]);
        exit(-1);
    }

    /* Get implementation pointer */
    auto implPtr = Interface::getImplementation();
    /* Initialization */
    auto ret = implPtr->initialize(
        configDir.value(),
        configFile.value());

    if (ret.code != ReturnCode::Success)
    {
        cerr << "[ERROR] initialize() returned error: " << ret.code << "." << endl
             << ret.info << endl;
        return FAILURE;
    }

    runQuality(implPtr, inputFile.value(), outputFile);

    return 0;
}
