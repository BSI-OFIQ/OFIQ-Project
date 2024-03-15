/**
 * @file test_conformance_table.cpp
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

#include <ofiq_lib.h>
//#include "test_constants.h"
#include "image_io.h"

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
//#include <utils.h>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>
#include <magic_enum.hpp>
#include <filesystem>

namespace fs = std::filesystem;

using namespace OFIQ;

// required helper functions
// - load CSV file
// - parse CSV file and create a list of quality assessment results 
//   for each row (face image file) in the table. 
//   Results should be stored in OFIQ::QualityAssessments,
//   defined as std::map<QualityMeasure, QualityMeasureResult>;
// - TEST: compare the list of measurements defined in the config file 
//   (will be ised for quality assessment computation) with the list 
//   of measuremente parsed from CSV file
// - for each list entry (face image):
//    - compute quality assessment results i.e. call 
//      OFIQ::vectorQuality(... OFIQ::FaceImageQualityAssessment& assessments)
//      the assessment results are then stored in assessments.qAssessments
//    - for each QualityMeasureResult compare (ASSERT_EQ) 
//      rawScore and scalar with conformance table results (from CSV)
// 

static std::string OFIQ_LIB_CONFIG_DIR{ "../../../data" };
static std::string OFIQ_LIB_CONFIG_FILE{ "ofiq_config.jaxn" };
static std::string CONFORMANCE_TABLE_CSV{ "../../../data/tests/expected_results/expected_results.csv" };

static std::shared_ptr<OFIQ::Interface> ofiqImplInstance;
static OFIQ::ReturnStatus ofiqInitResult;

static std::map<std::string, OFIQ::FaceImageQualityAssessment> assessmentsCache;

struct FaceImageAssessments {
	std::string imageFile;
	OFIQ::QualityAssessments qAssessments;
};

std::shared_ptr<OFIQ::Interface> getOfiqImplInstance(
	const std::string& configDir, const std::string& configFile);
std::vector<FaceImageAssessments> loadConformanceTable(
	const std::string& conformanceTableCSV);

std::vector<std::string> getNextLineAndSplitIntoTokens(
	std::istream& str,
	const char delim = ';');

std::vector<std::string> parseCSVColumnNames(
	const std::vector<std::string>& tokens,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar);

FaceImageAssessments faceImageAssessmentFromStrings(
	const std::vector<std::string>& strings,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar);

std::vector<std::string> parseConformanceTableHeader(
	std::istream& istr,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar);

std::vector<FaceImageAssessments> parseConformanceTableCSV(
	std::istream& istr,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar);

std::vector<std::tuple<std::string, OFIQ::QualityMeasure, double, double>> splitToSingleResults(
	const std::vector<FaceImageAssessments>& imageAssessments);

static std::vector<FaceImageAssessments> imageAssessments = loadConformanceTable(CONFORMANCE_TABLE_CSV);

std::shared_ptr<OFIQ::Interface> getOfiqImplInstance(
	const std::string& configDir, const std::string& configFile)
{
	if (!ofiqImplInstance)
	{
		ofiqImplInstance = OFIQ::Interface::getImplementation();
		printf("Initializing OFIQ using config directory %s and config file %s\n",
			configDir.c_str(), configFile.c_str());
		ofiqInitResult = ofiqImplInstance->initialize(configDir, configFile);
	}
	else
		printf("Reusing existing OFIQ instance\n");

	return ofiqImplInstance;
}

std::vector<FaceImageAssessments> loadConformanceTable(
	const std::string& conformanceTableFile)
{
	printf("Loading conformance table from: %s\n", conformanceTableFile.c_str());
	std::ifstream ifs(conformanceTableFile);

	std::map<OFIQ::QualityMeasure, int> colIdxMeasureRaw;
	std::map<OFIQ::QualityMeasure, int> colIdxMeasureScalar;

	std::vector<std::string> invalid_column_names =
		parseConformanceTableHeader(
			ifs,
			colIdxMeasureRaw,
			colIdxMeasureScalar);

	//if (!invalid_column_names.empty())
	//{
	//	printf("Invalid column names detected:\n");
	//	for (const auto& invalid_column : invalid_column_names)
	//	{
	//		printf("\t'%s'\n", invalid_column.c_str());
	//	}
	//}

	return parseConformanceTableCSV(
		ifs, colIdxMeasureRaw, colIdxMeasureScalar);
}


TEST(TestDataValidity, ConfigDirValid)
{
	fs::path configDirPath = fs::weakly_canonical(fs::path(OFIQ_LIB_CONFIG_DIR));
	ASSERT_TRUE(fs::exists(configDirPath)) << "Config directory not found: " <<
		configDirPath.string() << ", originally: " << OFIQ_LIB_CONFIG_DIR << std::endl;
}

TEST(TestDataValidity, ConfigFileValid)
{
	fs::path configDirPath(OFIQ_LIB_CONFIG_DIR);
	fs::path fullConfPath = fs::weakly_canonical(configDirPath / fs::path(OFIQ_LIB_CONFIG_FILE));
	ASSERT_TRUE(fs::exists(fullConfPath)) << "Config file '" << OFIQ_LIB_CONFIG_FILE <<
		"' not found in the directory: " << OFIQ_LIB_CONFIG_DIR << std::endl;

	std::ifstream istream(fullConfPath.string());
	ASSERT_TRUE(istream.good()) << "Can't read the config file: " << fullConfPath.string() << std::endl;
}

TEST(TestDataValidity, ConformanceTableValid)
{
	fs::path fullConfTablePath = fs::weakly_canonical(fs::path(CONFORMANCE_TABLE_CSV));
	ASSERT_TRUE(fs::exists(fullConfTablePath)) <<
		"Conformnce table file not found: " << fullConfTablePath <<
		", originally: " << CONFORMANCE_TABLE_CSV << std::endl;

	for (auto& imageResults : imageAssessments)
	{
		fs::path fullImgPath = fs::weakly_canonical(fs::path(imageResults.imageFile));
		ASSERT_TRUE(fs::exists(fullImgPath)) <<
			"Can't read test image file: " << fullImgPath.string() << std::endl;
	}


}

class ConformanceTest :public ::testing::TestWithParam<std::tuple<std::string, OFIQ::QualityMeasure, double, double>>
{
protected:
    static std::shared_ptr<OFIQ::Interface> ofiqImpl;

    static void SetUpTestSuite()
    {
		ofiqImpl = getOfiqImplInstance(OFIQ_LIB_CONFIG_DIR, OFIQ_LIB_CONFIG_FILE);

		ASSERT_EQ(ofiqInitResult.code, OFIQ::ReturnCode::Success);
    }
};


std::shared_ptr<OFIQ::Interface> ConformanceTest::ofiqImpl;

TEST_P(ConformanceTest, ValidateScores)
{
	std::string imageFile = std::get<0>(GetParam());
	OFIQ::QualityMeasure measure = std::get<1>(GetParam());
	double rawScore = std::get<2>(GetParam());
	double scalarScore = std::get<3>(GetParam());

	OFIQ::FaceImageQualityAssessment assessments;

	std::string filename = fs::path(imageFile).filename().string();
	
	if (auto cacheIter = assessmentsCache.find(filename);
			cacheIter != assessmentsCache.end())
		assessments = cacheIter->second;
	else
	{
		Image inputImage;
		ReturnStatus retStatus = OFIQ_LIB::readImage(imageFile, inputImage);
		ASSERT_TRUE(retStatus.code == OFIQ::ReturnCode::Success);
		ofiqImpl->vectorQuality(inputImage, assessments);
		assessmentsCache[filename] = assessments;
	}

	auto iter = assessments.qAssessments.find(measure);
	ASSERT_TRUE(iter != assessments.qAssessments.end());
	QualityMeasureResult measureResult = iter->second;
	
	//EXPECT_NEAR(measureResult.rawScore, rawScore, 1e-3);
	ASSERT_NEAR(measureResult.scalar, scalarScore, 1.0) << "scalar scores deviate by more than 1" << std::endl;
	ASSERT_EQ(measureResult.scalar, round(measureResult.scalar)) << "scalar scores have to be integer" << std::endl;
}

std::string generateTestname(const testing::TestParamInfo<ConformanceTest::ParamType>& info) {
	std::string imageFile = std::get<0>(info.param);
	OFIQ::QualityMeasure measure = std::get<1>(info.param);
	auto measureStr = static_cast<std::string>(magic_enum::enum_name(measure));
	std::string name = fs::path(imageFile).filename().string() + measureStr;
	name.erase(std::remove_if(name.begin(), name.end(),
		[](auto const& c) -> bool { return !std::isalnum(c); }), name.end());
	return name;
}

INSTANTIATE_TEST_SUITE_P(
	ConformanceTableTests,
	ConformanceTest,
	::testing::ValuesIn(
		splitToSingleResults(imageAssessments)
	),
	generateTestname
);

//
// Helper functions for parsing conformance table
//

std::vector<std::tuple<std::string, OFIQ::QualityMeasure, double, double>> splitToSingleResults(
	const std::vector<FaceImageAssessments>& faceimageAssessments)
{
	std::vector<std::tuple<std::string, OFIQ::QualityMeasure, double, double>> singleResults;
	for (auto& imageResults : faceimageAssessments)
	{
		for (auto& [measure, measureResult] : imageResults.qAssessments)
		{
			singleResults.emplace_back(
					imageResults.imageFile,
					measure,
					measureResult.rawScore,
					measureResult.scalar
			);

		}
	}
	return singleResults;
}

std::vector<std::string> getNextLineAndSplitIntoTokens(
	std::istream& istr,
	const char delim)
{
	std::vector<std::string>   result;
	std::string                line;
	std::getline(istr, line);

	std::stringstream          lineStream(line);
	std::string                cell;

	while (std::getline(lineStream, cell, delim))
	{
		result.emplace_back(cell);
	}
	// This checks for a trailing comma with no data after it.
	if (!lineStream && cell.empty())
	{
		// If there was a trailing comma then add an empty element.
		result.emplace_back("");
	}
	return result;
}

std::vector<std::string> parseCSVColumnNames(
	const std::vector<std::string>& tokens,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar)
{
	std::vector<std::string> invalid_names;
	for (int i = 0; i < tokens.size(); i++)
	{
		std::string measureName = tokens[i];
		std::string::size_type pos = measureName.find(".scalar");
		bool isScalar = false;
		if (pos != std::string::npos)
		{
			isScalar = true;
			measureName = measureName.substr(0, pos);
		}

		auto measure = magic_enum::enum_cast<OFIQ::QualityMeasure>(measureName);
		if (measure.has_value())
		{
			if(isScalar)
				colIdxMeasureScalar[measure.value()] = i;
			else
				colIdxMeasureRaw[measure.value()] = i;
		}
		else
			invalid_names.emplace_back(tokens[i]);
	}
	return invalid_names;
}

FaceImageAssessments faceImageAssessmentFromStrings(
	const std::vector<std::string>& strings,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar)
{
	FaceImageAssessments assessments;
	assessments.imageFile = strings[0];

	for (const auto& [measure, idx] : colIdxMeasureRaw)
	{
		double val = std::stod(strings[idx]);
		assessments.qAssessments[measure] = OFIQ::QualityMeasureResult(val);
	}

	for (const auto& [measure, idx] : colIdxMeasureScalar)
	{
		double val = std::stod(strings[idx]);
		assessments.qAssessments[measure].scalar = val;
	}

	return assessments;
}


std::vector<std::string> parseConformanceTableHeader(
	std::istream& istr,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar)
{
	std::vector<std::string> tokens = getNextLineAndSplitIntoTokens(istr);

	std::vector<std::string> invalid_column_names =
		parseCSVColumnNames(tokens, colIdxMeasureRaw, colIdxMeasureScalar);

	return invalid_column_names;
}

std::vector<FaceImageAssessments> parseConformanceTableCSV(
	std::istream& istr,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureRaw,
	const std::map<OFIQ::QualityMeasure, int>& colIdxMeasureScalar)
{
	std::vector<FaceImageAssessments> assessments;

	int takeFirstN = INT32_MAX; // 3
	while (istr && takeFirstN > 0)
	{
		takeFirstN--;
		std::vector<std::string> tokens = getNextLineAndSplitIntoTokens(istr);
		if (tokens.size() < colIdxMeasureRaw.size()+ colIdxMeasureScalar.size())
		{
			// we have an invalid number of tokens
			continue;
		}
		
		assessments.emplace_back(
			faceImageAssessmentFromStrings(
				tokens, colIdxMeasureRaw, colIdxMeasureScalar));
	}
	return assessments;
}

int main(int argc, char* args[])
{
	::testing::InitGoogleTest(&argc, args);

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(args[i], "-c") == 0 && i + 1 < argc)
			OFIQ_LIB_CONFIG_DIR = args[++i];
		else if (strcmp(args[i], "-cf") == 0 && i + 1 < argc)
			OFIQ_LIB_CONFIG_FILE = args[++i];
		else if (strcmp(args[i], "-r") == 0 && i + 1 < argc)
			CONFORMANCE_TABLE_CSV = args[++i];
	}

	imageAssessments = loadConformanceTable(CONFORMANCE_TABLE_CSV);

	return RUN_ALL_TESTS();
}
