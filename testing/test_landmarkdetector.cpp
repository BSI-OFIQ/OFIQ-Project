#include <fstream>
#include <filesystem>
#include <cstdlib>

#include <tao/json.hpp>
#include <gtest/gtest.h>


#include <faceqa_lib.h>

#include <Configuration.h>
#include <opencv_ssd_face_detector.h>
#include <AllLandmarks.h>
#include <Session.h>
#include <image_io.h>

#include "test_constants.h"

using namespace FACEQA_LIB;
using namespace FACEQA_LIB::modules::detectors;
using namespace FACEQA_LIB::modules::landmarks;

namespace fs = std::filesystem;

using jType = tao::json::type;

TEST(LandmarkDetector, ADNetLandmarks)
{

  /* Get implementation pointer */
  std::string configDir, configFile;
  configFile = FACEQA_LIB_CONFIG_FILE_ADNET;
  configDir = FACEQA_LIB_CONFIG_DIR;

  fs::path fullConfPath, pathConfFilename, pathConfDir(configDir);
  // use default fileName 'faceqa.jaxn' if no configValue was given
  pathConfFilename = fs::path(configFile);
  fullConfPath = fs::weakly_canonical(pathConfDir / pathConfFilename);

  std::ifstream istream(fullConfPath.string());

  std::string source;
  tao::json::value jsonValue = tao::json::jaxn::from_stream(istream, source);


  Configuration config(configDir, configFile);

  SSDFaceDetector faceDetector(config);

  // get expected results from the test configuration
  std::string inputImageFile = jsonValue["Test.ADNetLandmarks"]["input_image"].get_string();
  unsigned int expected_number_of_landmarks =
      jsonValue["Test.ADNetLandmarks"]["number_of_landmarks"].get_unsigned();
  // bounding box
  unsigned int bb_xleft =
      jsonValue["Test.ADNetLandmarks"]["face_bounding_box"]["xleft"].get_unsigned();
  unsigned int bb_ytop =
      jsonValue["Test.ADNetLandmarks"]["face_bounding_box"]["ytop"].get_unsigned();
  unsigned int bb_width =
      jsonValue["Test.ADNetLandmarks"]["face_bounding_box"]["width"].get_unsigned();
  unsigned int bb_height =
      jsonValue["Test.ADNetLandmarks"]["face_bounding_box"]["height"].get_unsigned();
  // landmarks
  auto landmarksVec = jsonValue["Test.ADNetLandmarks"]["landmarks"].get_array();
  unsigned int landmarks_tolerance =
      jsonValue["Test.ADNetLandmarks"]["landmarks_tolerance"].get_unsigned();


  FACEQA::Image inputImage;
  FACEQA::ReturnStatus ret = readImage(inputImageFile, inputImage);

  EXPECT_TRUE(ret.code == FACEQA::ReturnCode::Success);

  FACEQA::FaceImageQualityAssessment assessment;

  Session session(inputImage, assessment);

  std::vector<FACEQA::BoundingBox> foundFaces = faceDetector.detectFaces(session);

  EXPECT_TRUE(foundFaces.size() == 2);
  EXPECT_TRUE(foundFaces[0].faceDetector == FACEQA::FaceDetectorType::OPENCVSSD);

  EXPECT_EQ(foundFaces[0].xleft, bb_xleft);
  EXPECT_EQ(foundFaces[0].ytop, bb_ytop);
  EXPECT_EQ(foundFaces[0].width, bb_width);
  EXPECT_EQ(foundFaces[0].height, bb_height);

  ADNetFaceLandmarkExtractor landmarkExctractor(config, faceDetector);
  auto detectedLandmarks = landmarkExctractor.extractLandmarks(session);


  EXPECT_TRUE(FACEQA::LandmarkType::ADNet == detectedLandmarks.type);
  EXPECT_EQ(expected_number_of_landmarks, detectedLandmarks.landmarks.size());


   for (int i = 0; i < detectedLandmarks.landmarks.size(); i++) {
    unsigned int delta_x = std::abs(detectedLandmarks.landmarks[i].x - (int16_t)landmarksVec[i]["x"].get_unsigned());
    unsigned int delta_y = std::abs(detectedLandmarks.landmarks[i].y - (int16_t)landmarksVec[i]["y"].get_unsigned());
       EXPECT_LE(
           delta_x,
           landmarks_tolerance);
       EXPECT_LE(
            delta_y,
           landmarks_tolerance);
        //std::cout << "i: " << i << "  x: " << detectedLandmarks.landmarks[i].x
        //         << "   y: " << detectedLandmarks.landmarks[i].y 
        //   << " -- expected x: " << landmarksVec[i]["x"].get_unsigned()
        //         << " y: " << landmarksVec[i]["y"].get_unsigned()
        //   << std::endl;
   }
}






