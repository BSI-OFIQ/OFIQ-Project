/**
 * @file OFIQSampleApp.cpp
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
 * @author OFIQ development team
 */

#if defined _WIN32 && defined OFIQ_EXPORTS
#undef OFIQ_EXPORTS
#endif

#include "ofiq_lib.h"
#include "image_io.h"
#include "utils.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <magic_enum.hpp>
#include <filesystem>

constexpr int SUCCESS = 0;
constexpr int FAILURE = 1;


namespace fs = std::filesystem;

using namespace std;
using namespace OFIQ;
using namespace OFIQ_LIB;


int getQualityAssessmentResults(
    std::shared_ptr<Interface>& implPtr,
    const string& inputFile,
    FaceImageQualityAssessment& assessments);

std::vector<std::string> readFileLines(
    const std::string& inputFile);

string exportAssessmentResultsToString(
    const FaceImageQualityAssessment& assessments,
    bool doExportScalar = false);

std::vector<std::string> readFileLines(const std::string& inputFile)
{
    std::vector<std::string> filenames;
    std::ifstream ifs(inputFile);
    std::string line;

    while(ifs)
    {
        std::getline(ifs, line);
        // ignore empty lines and comment lines
        if (line.empty() || line.at(0) == '#')
            continue;
        filenames.push_back(line);
    }

    return filenames;
}

std::string to_lower(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return data;
}

std::vector<std::string> readImageFilesFromDirectory(const std::string& inputDirectory)
{
    std::vector<std::string> filenames;

    for (const auto& entry : fs::directory_iterator(inputDirectory))
    {
        std::string extension = to_lower(entry.path().extension().generic_string());
        if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
        {
            filenames.push_back(entry.path().generic_string());
        }
    }

    return filenames;
}

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
    std::shared_ptr<Interface>& implPtr, const string& inputFile, std::ostream* p_outStream = &std::cout )
{
    std::vector<std::string> imageFiles;
    std::vector<FaceImageQualityAssessment> faceImageQAs;
    std::vector<int> faceImageQAresultCodes;

    fs::path inputFilePath(inputFile);

    if (fs::is_directory(fs::path(inputFile)))
    {
        imageFiles = readImageFilesFromDirectory(inputFile);
    }
    else if (std::string fileExt = inputFilePath.extension().string();
        isStringContained({ ".txt", ".csv" }, fileExt))
        // a list of image files
        imageFiles = readFileLines(inputFile);
    else
        // single image file
        imageFiles.push_back(inputFile);

    // process image file(s)
    for (auto const& imageFile: imageFiles)
    {
        FaceImageQualityAssessment assessment;
        int resCode = getQualityAssessmentResults(implPtr, imageFile, assessment);
        faceImageQAresultCodes.push_back(resCode);

        faceImageQAs.push_back(assessment);

        string strQAresRaw = exportAssessmentResultsToString(
            assessment, false);
        string strQAresScalar = exportAssessmentResultsToString(
            assessment, true);

        cout << "raw scores:" << strQAresRaw << std::endl;
        cout << "scalar scores:" << strQAresScalar << std::endl;

    }

    if (faceImageQAs.empty())
    {
        cerr << "[ERROR] " << "empty result list" << "." << endl;
        return FAILURE;
    }

    if (faceImageQAs.size() != imageFiles.size())
    {
        cerr << "[ERROR] " << "invalid number of measurement results. Is " << imageFiles.size() <<
            ", has to be " << faceImageQAs.size() << endl;
        return FAILURE;
    }

    // export to csv file, output format is the folliwing
    // "Filename", MeasurementName1, ..., MeasurementNameN, MeasurementName1.scalar, ..., MeasurementNameN.scalar
    // Filename,      Measurement1.raw, ..., MeasurementN.raw, Measurement1.scalar, ..., MeasurementN.scalar
    vector<string> measureNames;
    vector<string> measureNamesScalar;
    for (const auto& [measure, measure_result]: faceImageQAs[0].qAssessments)
    {
        auto mName = static_cast<std::string>(magic_enum::enum_name(measure));
        measureNames.push_back(mName);
        measureNamesScalar.push_back(mName+string(".scalar"));
    }

    *p_outStream << "Filename;";
    for(const auto& mn: measureNames)
        *p_outStream << mn << ';';
    for (const auto& mn : measureNamesScalar)
        *p_outStream << mn << ';';
    //for (size_t i = 0; i < measureNames.size(); i++)
    //    *outStream << measureNames[i] << ';';
    //for (size_t i = 0; i < measureNamesScalar.size(); i++)
    //    *outStream << measureNames[i] << ';';
    *p_outStream << std::endl;

    for (size_t i = 0; i < imageFiles.size(); i++)
    {
        string strQAresRaw = exportAssessmentResultsToString(
            faceImageQAs[i], false);
        string strQAresScalar = exportAssessmentResultsToString(
            faceImageQAs[i], true);
        
        *p_outStream << imageFiles[i] << ';' << strQAresRaw << ';' << strQAresScalar << std::endl;

#if 1                
        std::cout << "-------------------------------------------------------" << std::endl;
        std::cout << "Image file: '" << imageFiles[i] << "' has attributes:" << std::endl; 
        for (const auto& [measure, measure_result] : faceImageQAs[i].qAssessments)
        {
            auto mName = static_cast<std::string>(magic_enum::enum_name(measure));
            auto rawScore = measure_result.rawScore;
            auto scalarScore = measure_result.scalar;
            if (measure_result.code != QualityMeasureReturnCode::Success)
            {
                scalarScore = -1;
            }

            std::cout << mName << "-> rawScore:  " << rawScore << "   scalar: " << scalarScore
                      << std::endl;
        }
        std::cout << "-------------------------------------------------------" << std::endl;
#endif        
    }
    
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

    std::cout << "--> Start processing image file: " << inputFile << std::endl;
    retStatus = implPtr->vectorQuality(image, assessments);

    return retStatus.code == ReturnCode::Success ? SUCCESS : FAILURE;
}

string exportAssessmentResultsToString(
    const FaceImageQualityAssessment& assessments, 
    bool doExportScalar)
{
    std::string resultStr;
    for ( auto it = assessments.qAssessments.begin() ; it != assessments.qAssessments.end() ; )
    {
        const auto& [measure, measure_result] = *it;
        const QualityMeasureResult& qaResult = measure_result;

        double scalarScore;
        if (measure_result.code != QualityMeasureReturnCode::Success)
        {
            scalarScore = -1.0;
        }
        else
        {
            scalarScore = qaResult.scalar;
        }

        if (double val = doExportScalar ? scalarScore : qaResult.rawScore;
            round(val) == val)
        {
            resultStr += to_string((int)val);
        }
        else
            resultStr += to_string(val);

        if (++it != assessments.qAssessments.end())
        {
            resultStr += ';';
        }
    }
    // replace decimal point for german MS Excel
    //std::replace(resultStr.begin(), resultStr.end(), '.', ',');    
    return resultStr;
}


void usage(const string& executable)
{
    cerr << "Usage: " << executable
         << " -c configDir "
            "-o outputFile -h outputStem -i inputFile -t numForks"
         << endl;
}

int main(int argc, char* argv[])
{
    int requiredArgs = 1; /* exec name */
    if (argc < requiredArgs)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    string configDir{ "config" };
    char* outputFile = nullptr;
    string inputFile;
    string configFile;

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
            return FAILURE;
        }
    }

    if (fs::is_regular_file(configDir))
    {
        if (!configFile.empty())
        {
            cerr << "[ERROR] Redundant specification of configuration file." << endl;
            return FAILURE;
        }

        configFile = fs::path(configDir).filename().generic_string();
        configDir = fs::path(configDir).parent_path().generic_string();
    }

    /* Get implementation pointer */
    auto implPtr = Interface::getImplementation();
    /* Initialization */
    auto ret = implPtr->initialize(
        configDir,
        configFile);

    if (ret.code != ReturnCode::Success)
    {
        cerr << "[ERROR] initialize() returned error: " << ret.code << "." << endl
             << ret.info << endl;
        return FAILURE;
    }

    // write to output file
    if (outputFile != nullptr)
    {
        std::ofstream ofs(outputFile);
        if (ofs.good())
        {
            runQuality(implPtr, inputFile, &ofs);
        }
        else
        {
        cerr << "[ERROR] Could not open '" << outputFile << "'." << endl
             << ret.info << endl;
        return FAILURE;
        }
    }
    else
    {
        runQuality(implPtr, inputFile, &std::cout);
    }

    return 0;
}
