/**
 * @file mainpage.h 
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
 * @brief This header file is for generating the doxygen documentation
 * for OFIQ.
 * @author OFIQ development team
 *
 * @mainpage Open Source Face Image Quality (OFIQ) Library
 * @section sec_intro Introduction
 * OFIQ (Open Source Face Image Quality) is a software library for computing quality 
 * aspects of a facial image. OFIQ is written in the C/C++ programming language.
 * OFIQ is the reference implementation for the ISO/IEC 29794-5 international
 * standard; see [https://bsi.bund.de/dok/OFIQ-e](https://bsi.bund.de/dok/OFIQ-e).
 * 
 * @includedoc "../../LICENSE.md"
 * 
 * @section sec_compilation Compilation
 * To build OFIQ you need to install Python with pip, cmake and conan.
 * <ul>
 *  <li>Python (version 3.10.12 or higher)</li>
 *  <li>[Download and install cmake](https://cmake.org/) (version 3.26 or higher)</li>
 *  <li>[Download and install conan](https://conan.io/) (version 2.0.17)</li>
 * </ul>
 *
 * @subsection sec_compilation_linux Linux
 * The following has been tested on an Ubuntu 22.04 (64 bit) installation.
 * <br/>
 * Install necessary packages.
 * <pre>
 * $ sudo apt-get install build-essential python3-pip liblapack-dev
 * </pre>
 * To install cmake (in a version 3.26 or higher) use snap (and not apt) as follows.
 * <pre>
 * $ sudo snap install cmake --classic
 * </pre>
 * Conan is installed via Python with 
 * <pre>
 * pip install conan==2.0.17
 * </pre>
 * In order to build OFIQ and installing all required packages via conan run the following.
 * <pre>
 * $ cd <OFIQ-SOURCE>/scripts
 * $ sh build.sh
 * </pre>
 * where <OFIQ-SOURCE> denotes the root folder of the OFIQ source files.
 * <br/>
 * This will create the following output.
 * <table>
 *  <tr>
 *   <td><b>file/directory</b></td>
 *   <td><b>description</b></td>
 *  </tr>
 *  <tr>
 *   <td>build/</td>
 *   <td>Folder with the Linux build including the binaries <code>OFIQSampleApp</code> and <code>libofiq_lib.so</code>.</td>
 *  </tr>
 *  <tr>
 *   <td>build/conan/</td>
 *   <td>Conan cache with packages downloaded.</td>
 *  </tr>
 *  <tr>
 *   <td>install_x86_64_linux/</td>
 *   <td>Folder with the installation including the binaries <code>Release/bin/OFIQSampleApp</code>, 
 *   <code>Release/lib/libofiq_lib.so</code>, <code>Release/lib/libonnxruntime.so.1.14.1</code>, and the 
 *   header files in <code>Release/include/</code>. </td>
 *  </tr>
 *  <tr>
 *   <td>data/models/</td>
 *   <td>Model files downloaded from the ISO portal during build process (see @ref sec_download_models).</td>
 *  </tr>
 *  <tr>
 *   <td>data/tests/images/</td>
 *   <td>Conformance test images downloaded from the ISO portal (see @ref sec_download_conformance_test_images).</td>
 *  </tr>
 * </table>
 * 
 * @subsection sec_compilation_windows Windows
 * The following has been tested on a Windows 10 (64 bit) installation using a <b>Python installation version 3.11.5</b> 
 * with pip package such that the <code>pip</code> command can be executed from the command prompt. Furthermore, an installation
 * of <b>cmake version 3.29</b> has been used. As the compiler, <b>Microsoft's Visual Studio 2019</b> was used.
 * <br/>
 * <br/>
 * To install conan, run
 * <pre>
 * $ pip install conan==2.0.17
 * </pre>
 * from the command prompt.
 * <br/>
 * <br/>
 * In order to build OFIQ and install all required packages run the following.
 * <pre>
 * $ cd <OFIQ-SOURCE>\scripts\
 * $ .\build.cmd
 * </pre>
 * where <OFIQ-SOURCE> denotes the root folder of the OFIQ source files.
 * <br/>
 * This will create the following output.
 * <table>
 *  <tr>
 *   <td><b>file/directory</b></td>
 *   <td><b>description</b></td>
 *  </tr>
 *  <tr>
 *   <td>build\\build_win\\</td>
 *   <td>Folder with the Visual Studio solution files placed and pre-compilation.</td>
 *  </tr>
 *  <tr>
 *   <td>build\\conan\\</td>
 *   <td>Conan cache with packages downloaded.</td>
 *  </tr>
 *  <tr>
 *   <td>install_x86_64\\</td>
 *   <td>Folder with the OFIQ installation files. This includes the binaries <code>Release\\bin\\OFIQSampleApp</code>, 
 *   <code>Release\\bin\\libofiq_lib.so</code>, <code>Release\\bin\\libonnxruntime.so.1.14.1</code>, and the 
 *   header files in <code>Release\\include\\</code>.</td>
 *  </tr>
 *  <tr>
 *   <td>data\\models\\</td>
 *   <td>Model files downloaded from the ISO portal during build process (see @ref sec_download_models).</td>
 *  </tr>
 *  <tr>
 *   <td>data\\tests\\images\\</td>
 *   <td>Conformance test images downloaded from the ISO portal (see @ref sec_download_conformance_test_images).</td>
 *  </tr>
 * </table>
 * 
 * @subsection sec_download_models Download model files
 * To run OFIQ, the model files from [the ISO portal](https://standards.iso.org/iso-iec/29794/-5/ed-1/en/OFIQ-MODELS.zip) 
 * need to be downloaded and be placed in the <code>./data/models/</code> directory. 
 * This step is integrated in the cmake building process.
 * 
 * @subsection sec_download_conformance_test_images Download conformance test images
 * To run conformance tests, the conformance test
 * images need to be downloaded from 
 * [the ISO portal](https://standards.iso.org/iso-iec/29794/-5/ed-1/en/OFIQ-IMAGES.zip) 
 * and be placed in the <code>./data/tests/images/</code> directory.
 * This step is integrated in the cmake building process.
 *
 * @section sec_conformance_test Running conformance tests
 * The conformance tests are executed by going to <code><OFIQ-SOURCE>/scripts/</code> 
 * and run <code>conformance_tests.cmd</code> (Windows) or 
 * <code>conformance_tests.sh</code> (Linux).
 * 
 * @section sec_sample Running the sample executable
 * In this section, we describe how to run the sample application of OFIQ after
 * compilation (see @ref sec_compilation). A documentation for the arguments that
 * can be passed can be found @ref sec_sample_args "below".
 * 
 * @subsection sec_simple_sample Quality assessment for a single facial image
 * The sample application takes an images and outputs the computed 
 * quality assessments. For example, to output the quality 
 * assessments for one of the conformance test images using OFIQ's configuration (in
 * <code>./data</code>), run the following commands on Linux.
 * <pre>
 * $ cd <OFIQ-SOURCE>/install_x86_64_linux/Release/bin/
 * $ ./OFIQSampleApp -c ../../../data/ofiq_config.jaxn -i 
 *         ../../../data/tests/images/b-01-smile.png
 * </pre>
 * On Windows run the following commands.
 * <pre>
 * $ cd <OFIQ-SOURCE>\\install_x86_64\\Release\\bin
 * $ .\OFIQSampleApp -c ..\\..\\..\\data\\ofiq_config.jaxn -i 
 *         ..\\..\\..\\data\\tests\\images\\b-01-smile.png
 * </pre>
 * 
 * @subsection sec_batch_sample Quality assessment for multiple images
 * To reproduce the conformance test table given in Annex A of the ISO/IEC 29794-5
 * international standard for all conformance test images, run the following commands on Linux.
 * <pre>
 * $ cd <OFIQ-SOURCE>/install_x86_64_linux/Release/bin/
 * $ ./OFIQSampleApp -c ..\\..\\..\\data\\ofiq_config.jaxn -i 
 *         ..\\..\\..\\data\\tests\\images\\ -o table.csv
 * </pre>
 * The result will be written in the file 
 * <code><OFIQ-SOURCE>/install_x86_64_linux/Release/bin/table.csv</code>. 
 * <br/><br/>
 * On Windows run the following commands.
 * <pre>
 * $ cd <OFIQ-SOURCE>\\install_x86_64\\Release\\bin\\
 * $ .\\OFIQSampleApp -c ..\\..\\..\\data\\ -i 
 *          ..\\..\\..\\data\\tests\\images\\ -o table.csv
 * </pre> 
 * The result will be written in the file 
 * <code><OFIQ-SOURCE>\\install_x86_64\\Release\\bin\\table.csv</code>. 
 * 
 * @subsection sec_sample_args Arguments
 * The usage pattern of the sample application is the following.
 * <pre>
 *  OFIQSampleApp 
 *    -c <directory or file path> 
 *    [-cf <config file name>] 
 *    -i <directory or image file path> 
 *    [-o <csv file path>]
 * </pre>
 * The following table documents the usage of the sample application.
 * <table>
 *  <tr>
 *    <td><b>flag</b></td>
 *    <td><b>argument</b></td>
 *  </tr>
 *  <tr>
 *    <td>-c</td>
 *    <td>Path to a directory containing the file ofiq_config.jaxn or a path to a JAXN configuration file (see @ref sec_config).</td>
 *  </tr>
 *  <tr>
 *    <td>-cf</td>
 *    <td>Name of the JAXN configuration file contained in the directory specified by the flag -c. Must be omitted if -c specifies a path to a file.</td>
 *  </tr>
  *  <tr>
 *    <td>-i</td>
 *    <td>Path to a directory containing facial images or a path to a facial image file. If a directory path is specified, all images in PNG and JPEG format will be processed.</td>
 *  </tr>
  *  <tr>
 *    <td>-o</td>
 *    <td>Path to a CSV file to where the quality assessment is written. If -o is not specified, the output is written to the standard output.</td>
 *  </tr>
 * </table>
 * 
 * @section sec_config Configuration
 * In this section, we describe the configuration file for OFIQ.
 * OFIQ uses a JSON-like configuration based on the [taoJSON](https://github.com/taocpp/json) 
 * library. Using taoJSON, OFIQ reads its configuration from a
 * [JAXN](https://github.com/stand-art/jaxn)-formatted file.
 * <br/>
 * <br/>
 * A minimal configuration working with OFIQ looks as follows.
 * <pre>
{
 "config": {
  "detector": "ssd",
  "landmarks": "ADNet",
  "measures": [
   "UnifiedQualityScore"
  ],
  "params": {
   "detector": {
    "ssd": {
     "model_path": "models/face_detection/ssd_facedetect.caffemodel",
     "prototxt_path": "models/face_detection/ssd_facedetect.prototxt.txt",
     "confidence_thr": 0.4,
     "min_rel_face_size": 0.01,
     "padding": 0.2
    }
   },
   "landmarks": {
    "ADNet": {
     "model_path": "models/face_landmark_estimation/ADNet.onnx"
    }
   },
   "measures": {
    "UnifiedQualityScore": {
     "model_path": "models/unified_quality_score/magface_iresnet50_norm.onnx"
    },
    "HeadPose": {
     "model_path": "models/head_pose_estimation/mb1_120x120.onnx"
    },
    "FaceOcclusionSegmentation": {
     "model_path": "models/face_occlusion_segmentation/face_occlusion_segmentation_ort.onnx"
    },
    "FaceParsing": {
     "model_path": "models/face_parsing/bisenet_400.onnx"
    }
   }
  }
 }
}
 * </pre>
 * OFIQ's C/C++ library provides the class \link OFIQ_LIB::Configuration Configuration\endlink 
 * which is responsible for reading and managing JAXN configurations.
 * <br/><br/>
 * Note that the model paths are specified as paths relative to the directory of the 
 * JAXN configuration file. We assume that the file above is stored in <OFIQ-SOURCE>/data.
 * 
 * @subsection sec_facedetect_cfg Configuration of the face detector
 * The face detector (SSD) must
 * be configured explicitly:
 * <pre>
 * {
 *  "config": {
 *    "detector": "ssd",
 *    ...
 *  }
 * }
 * </pre>
 * Additionally, the path to the model file and other parameters need 
 * to be configured:
 * <pre>
 * {
 *  ...
 *    "params": {
 *     "detector": {
 *       "ssd": {
 *         "model_path": "models/face_detection/ssd_facedetect.caffemodel",
 *         "prototxt_path": "models/face_detection/ssd_facedetect.prototxt.txt",
 *         "confidence_thr": 0.4,
 *         "min_rel_face_size": 0.01,
 *         "padding": 0.2
 *       }
 *     },
    ...  
 * }
 * </pre>
 * A documentation on the parameters are given in the following table.
 * <table>
 *  <tr>
 *   <td><b>Parameter</b></td><td><b>Description</b></td> 
 *  </tr>
 *  <tr>
 *   <td>model_path</td><td>path to the SSD model file in CAFEE format</td> 
 *  </tr>
 *  <tr>
 *   <td>prototxt_path</td><td>path to SSD's CAFFE protype file</td> 
 *  </tr>
 *  <tr>
 *   <td>confidence_thr</td><td>minimum value for the confidence 
 *   the detected faces; detected faces with 
 *   a lower confidence are discarded. Note, the specified value 0.4 (fixed for OFIQ) has 
 *   been determined experimentally.</td> 
 *  </tr>
 *  <tr>
 *   <td>min_rel_face_size</td><td>the minimum width of the face 
 *   bounding boxes relative to the width w of the input image;
 *   detected faces, with a bounding box width smaller than 
 *   min_rel_face_size*w are discarded. Note, the specified value 0.01 (fixed for OFIQ) has 
 *   been determined experimentally.</td> 
 *  </tr>
 *  <tr>
 *   <td>padding</td><td>horizontal and vertical padding of the
 *   original image prior face detection. Note, the specified value 0.2 (fixed for OFIQ) has 
 *   been determined experimentally.</td> 
 *  </tr>
 * </table>
 * 
 * @subsection sec_facelandmark_cfg Configuration of the landmark extractor
 * The face landmark extractor (ADNet) must be configured explicitly:
 * <pre>
 * {
 *  "config": {
 *    ...
 *    "landmarks": "ADNet",
 *    ...
 *  }
 * }
 * </pre>
 * Additionally, the path to the ADNet model file in ONNX format needs 
 * to be configured:
 * <pre>
 * {
 *  ...
 *    "params": {
 *     ...
 *     "landmarks": {
 *       "ADNet": {
 *         "model_path": "models/face_landmark_estimation/ADNet.onnx"
 *       }
 *     },
 *  ...  
 * }
 * </pre>
 * 
 * @subsection sec_required_cfg Other required configurations
 * As suggested by the minimal configuration file given in @ref sec_config, there
 * are model files that need to be configured and their configurations cannot be omitted.
 * These model files are detailed in the following table and shall be configured in 
 * the "config"."measures" environment.
 * <table>
 *  <tr><td><b>Algorithm</b></td><td><b>Description</b></td></tr>
 *  <tr><td>\link OFIQ_LIB::modules::measures::HeadPose HeadPose\endlink</td>
 *  <td></td>Head pose angles are pre-processed and used by some measures;
 *  therefore, the measure shall be configured. The path to the 
 *  [3DDFAV2](https://github.com/cleardusk/3DDFA_V2) model file 
 *  in ONNX format should be set using the <code>model_path</code> key.</tr>
 *  <tr><td>\link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation FaceOcclusionSegmentation\endlink</td>
 *  <td></td>Face occlusion segmentation pre-processing used by some measures assessing occlusion prevention. 
 *  The path to the  [FaceExtraction](https://github.com/face3d0725/FaceExtraction)
 *  model file in ONNX format should be set using the <code>model_path</code> key.
 *  NOTE: The OFIQ development team has been permitted by the FaceExtraction authors for 
 *  inclusion of the model in OFIQ without any restrictions; therefore, the referenced ONNX
 *  model file is subject to the OFIQ license agreement.</tr>
 *  <tr><td>\link OFIQ_LIB::modules::segmentations::FaceParsing FaceParsing\endlink</td>
 *   <td></td>Face parsing is pre-processed and used by some measures;
 *   therefore, the measure shall be configured (even if no measure is requested that uses
 *   the pre-processing result). The path to the 
 *   [BiSeNet](https://github.com/zllrunning/face-parsing.PyTorch) model file in ONNX format 
 *  should be set using the <code>model_path</code> key.</tr>
 * </table>
 * 
 * @subsection sec_requesting_measures Requesting measures
 * OFIQ implements a variety of measures for assessing properties of a facial
 * image. For a measure to be executed by OFIQ, it must be explicitly requested. 
 * For example, to only request the unified quality score measure, one can request it as follows.
 * <pre>
 * {
 *  "config": {
 *    ...
 *    "measures": [
 *     "UnifiedQualityScore"
 *    ],
 *    ...
 *  }
 * }
 * </pre>
 * At least one measure must be requested. An empty request list will
 * result in OFIQ throwing an error. A full list of requestable measures and its
 * <i>measure keys</i> can be found in the table of the 
 * @ref sec_default_config "default configuration section".
 * 
 * @subsection sec_default_config Default configuration
 * OFIQ is the reference implementation for the ISO/IEC 29794-5 standard. To reproduce the
 * conformance tests of the ISO/IEC 29794-5 standard one should use the (default) configuration provided
 * by the file <code><OFIQ-SOURCE>/data/ofiq_config.jaxn</code>; other configurations 
 * can be used; however, the resulting outputs of the quality assessment may not comply
 * with the ISO/IEC 29794-5 standard.
 * <br />
 * <br />
 * The entries of the default configuration are documented in the following table. 
 * Details on the algorithms can be found in the ISO/IEC 29794-5 document. Details on the 
 * implementations can be found in the OFIQ source code. Note, the QAA identifiers 
 * listed in the table are defined in ISO/IEC 29794-5.
 * 
 * <table>
 *  <tr>
 *  <td><b>QAA identifier</b></td>
 *  <td><b>description</b></td>
 *  <td><b>config key</b></td>
 *  <td><b>request key</b></td>
 *  <td><b>configuration parameters</b></td>
 *  <td><b>supports quality mapping config? - see details @ref sec_quality_config "here"</b></td>
 *  </tr>
 *  
 *  <tr>
 *  <td>-</td>
 *  <td>Face detector</td>
 *  <td>"config">"params".<br/>"detector"</td>
 *  <td>-</td>
 *  <td>see @ref sec_facedetect_cfg "here"</td>
 *  <td>-</td>
 *  </tr>
 *
 *  <tr>
 *  <td>-</td>
 *  <td>Face landmark estimator</td>
 *  <td>"config">"params".<br/>"landmarks"</td>
 *  <td>-</td>
 *  <td>see @ref sec_facelandmark_cfg "here"</td>
 *  <td>-</td>
 *  </tr>
 *
 *  <tr>
 *  <td>-</td>
 *  <td>Face parsing</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"FaceParsing"</td>
 *  <td>-</td>
 *  <td>see @ref sec_required_cfg "here"</td>
 *  <td>-</td>
 *  </tr> 
 *
 *  <tr>
 *  <td>-</td>
 *  <td>Face occlusion segmentation</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"FaceOcclusionSegmentation"</td>
 *  <td>-</td>
 *  <td>see @ref sec_required_cfg "here"</td>
 *  <td>-</td>
 *  </tr>
 *
 *  <tr>
 *  <td>-</td>
 *  <td>Landmarked region</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"FaceRegion"</td>
 *  <td>-</td>
 *  <td><code>alpha</code>: is 0 per default and only used for internal purposes</td>
 *  <td>-</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x41</td>
 *  <td>Unified quality score</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"UnifiedQualityScore"</td>
 *  <td>"config".<br/>"measures".<br/>"UnifiedQualityScore"</td>
 *  <td><code>model_path</code>: Path to an iResNet50 model file in ONNX format</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x42</td>
 *  <td>Background uniformity</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"BackgroundUniformity"</td>
 *  <td>"config".<br/>"measures".<br/>"BackgroundUniformity"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x43</td>
 *  <td>Illumination unformity</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"IlluminationUniformity"</td>
 *  <td>"config".<br/>"measures".<br/>"IlluminationUniformity"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x44</td>
 *  <td>Luminance brightness</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"Luminance"</td>
 *  <td>"config".<br/>"measures".<br/>"Luminance"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x45</td>
 *  <td>Luminance contrast</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"Luminance"</td>
 *  <td>"config".<br/>"measures".<br/>"Luminance"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x46</td>
 *  <td>Abscence of under-exposure</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"UnderExposurePrevention"</td>
 *  <td>"config".<br/>"measures".<br/>"UnderExposurePrevention"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x47</td>
 *  <td>Abscence of over-exposure</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"OverExposurePrevention"</td>
 *  <td>"config".<br/>"measures".<br/>"OverExposurePrevention"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x48</td>
 *  <td>Pixel intensity variation</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"DynamicRange"</td>
 *  <td>"config".<br/>"measures".<br/>"DynamicRange"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x49</td>
 *  <td>Sharpness</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"Sharpness"</td>
 *  <td>"config".<br/>"measures".<br/>"Sharpness"</td>
 *  <td><code>model_path</code>: Path to the random forest model file</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x4A</td>
 *  <td>Abscence of compression artifacts</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"NoCompressionArtifacts"</td>
 *  <td>"config".<br/>"measures".<br/>"NoCompressionArtifacts"</td>
 *  <td><code>model_path</code>: Path to OFIQ's compression artifact CNN in ONNX format</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x4B</td>
 *  <td>Colour naturality</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"NaturalColour"</td>
 *  <td>"config".<br/>"measures".<br/>"NaturalColour"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x4C</td>
 *  <td>Face uniqueness</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"SingleFacePresent"</td>
 *  <td>"config".<br/>"measures".<br/>"SingleFacePresent"</td>
 *  <td>none</td>
 *  <td>no</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x4D</td>
 *  <td>Eyes openess</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"EyesOpen"</td>
 *  <td>"config".<br/>"measures".<br/>"EyesOpen"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x4E</td>
 *  <td>Mouth closedness</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"MouthClosed"</td>
 *  <td>"config".<br/>"measures".<br/>"MouthClosed"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x4F</td>
 *  <td>Eyes visibility</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"EyesVisible"</td>
 *  <td>"config".<br/>"measures".<br/>"EyesVisible"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x50</td>
 *  <td>Mouth occlusion prevention</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"MouthOcclusionPrevention"</td>
 *  <td>"config".<br/>"measures".<br/>"MouthOcclusionPrevention"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x51</td>
 *  <td>Face occlusion prevention</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"FaceOcclusionPrevention"</td>
 *  <td>"config".<br/>"measures".<br/>"FaceOcclusionPrevention"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x52</td>
 *  <td>Inter-eye distance length</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"InterEyeDistance"</td>
 *  <td>"config".<br/>"measures".<br/>"InterEyeDistance"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x53</td>
 *  <td>Size of the head in the image</td>
 *  <td>"config".<br/>"params".<br/>"measures".<br/>"HeadSize"</td>
 *  <td>"config".<br/>"measures".<br/>"HeadSize"</td>
 *  <td>none</td>
 *  <td>yes - the argument to the quality mapping is \f$\|x-0.45\|\f$ where \f$x\f$ is the native quality score</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x54</td>
 *  <td>Leftward crop of the face image</td>
 *  <td>"config">"params".<br/>"measures".<br/>"CropOfTheFaceImage".<br/>"LeftwardCropOfTheFaceImage"</td>
 *  <td>"config".<br/>"measures".<br/>"CropOfTheFaceImage"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x55</td>
 *  <td>Rightward crop of the face image</td>
 *  <td>"config">"params".<br/>"measures".<br/>"CropOfTheFaceImage".<br/>"RightwardCropOfTheFaceImage"</td>
 *  <td>"config".<br/>"measures".<br/>"CropOfTheFaceImage"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x56</td>
 *  <td>Upward crop of the face image</td>
 *  <td>"config">"params".<br/>"measures".<br/>"CropOfTheFaceImage".<br/>"UpwardCropOfTheFaceImage"</td>
 *  <td>"config".<br/>"measures".<br/>"CropOfTheFaceImage"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x57</td>
 *  <td>Downward crop of the face image</td>
 *  <td>"config">"params".<br/>"measures".<br/>"CropOfTheFaceImage".<br/>"DownwardCropOfTheFaceImage"</td>
 *  <td>"config".<br/>"measures".<br/>"CropOfTheFaceImage"</td>
 *  <td>none</td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x58</td>
 *  <td>Pose angle yaw frontal alignment</td>
 *  <td>"config">-</td>
 *  <td>"config".<br/>"measures".<br/>"HeadPose"</td>
 *  <td>none</td>
 *  <td>no</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x59</td>
 *  <td>Pose angle pitch frontal alignment</td>
 *  <td>-</td>
 *  <td>"config".<br/>"measures".<br/>"HeadPose"</td>
 *  <td>none</td>
 *  <td>no</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x5A</td>
 *  <td>Pose angle roll frontal alignment</td>
 *  <td>-</td>
 *  <td>"config".<br/>"measures".<br/>"HeadPose"</td>
 *  <td>none</td>
 *  <td>no</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x5B</td>
 *  <td>Expression neutrality</td>
 *  <td>"config">"params".<br/>"measures".<br/>"ExpressionNeutrality"</td>
 *  <td>"config".<br/>"measures".<br/>"ExpressionNeutrality"</td>
 *  <td>
 *   <code>cnn_model_path1</code>: Path to the CNN model <i>enet_b0_8_best_vgaf_embed_zeroed.onnx</i> derived from 
 *   <a href="https://github.com/HSE-asavchenko/face-emotion-recognition">here</a> 
 *   in ONNX format
 *   <br/><br/>
 *   <code>cnn_model_path2</code>: Path to the CNN model <code>enet_b2_8_embed_zeroed.onnx</code> derived from 
 *   <a href="https://github.com/HSE-asavchenko/face-emotion-recognition">here</a> 
*    in ONNX format.
 *   <br/><br/>
 *   <code>adaboost_model_path</code>: Path to the AdaBoost classifier model file <code>hse_1_2_C_adaboost.yml.gz</code> from
 *   <a href="https://github.com/dasec/Efficient-Expression-Neutrality-Estimation">here</a>
 *  </td>
 *  <td>yes</td>
 *  </tr>
 *
 *  <tr>
 *  <td>0x5C</td>
 *  <td>Abscence of head coverings</td>
 *  <td>"config">"params".<br/>"measures".<br/>"NoHeadCovering"</td>
 *  <td>"config".<br/>"measures".<br/>"NoHeadCovering"</td>
 *  <td><code>threshold</code> - threshold between 0 and 1-/td>
 *  <td>no</td>
 *  </tr>
 * </table>
 * 
 * @subsection sec_quality_config Configuration of the quality mapping
 * Each measure implemented in OFIQ outputs a pair of values. The first
 * value is called <i>native quality score</i>. The second value is called
 * <i>quality component value</i> which is an integer between 0 (worst quality)
 * and 100 (best quality). The quality component value and is derived from 
 * the <i>native quality score</i> using a mapping function. Whether this mapping
 * function can be configured for the measure is stated in the table of 
 * section @ref sec_default_config.
 * <br/>
 * <br/>
 * Note, the OFIQ library implements hard-coded default quality 
 * mappings as a fallback.
 * <br/>
 * <br/>
 * A configurable quality mapping has the form of
 * \f[
 * Q(h,a,s,x,x_0,w)=h\cdot(a+s\cdot\mathrm{sigmoid}(x,x_0,w))
 * \f]
 * where
 * \f[
 * \mathrm{sigmoid}(x,x_0,w)=(1+\exp((x_0-x)/w)^{-1}.
 * \f]
 * Note, \f$x\f$ is the native quality score which is mapped to the quality component value.
 * The other symbols denote parameters that can be configured (see the example at the end of this section).
 * <table>
 *  <tr>
 *   <td><b>parameter</b></td>
 *   <td><b>description</b></td>
 *   <td><b>default value</b></td>
 *  </tr>
 *  <tr>
 *   <td>"h"</td>
 *   <td>scale factor</td>
 *   <td>100</td>
 *  </tr>
 *  <tr>
 *   <td>"a"</td>
 *   <td>constant shift</td>
 *   <td>0</td>
 *  </tr>
 *  <tr>
 *   <td>"s"</td>
 *   <td>signed weight for sigmoid part</td>
 *   <td>1</td>
 *  </tr>
 *  <tr>
 *   <td>"x0"</td>
 *   <td></td>center point within sigmoid function;
 *   the default value has been chosen arbitrarily and should specified when a mapping is configured.</td>
 *   <td>4</td>
 *  </tr>
 *  <tr>
 *   <td>"w"
 *   </td>
 *   <td>divisor within the sigmoid function; 
 *   the default value has been chosen arbitrarily and should specified when a mapping is configured.</td>
 *   <td>0.7</td>
 *  </tr>
 *  <tr>
 *   <td>"round"</td>
 *   <td>applies the compiler's native rounding function (<code>std::round</code>) such that only integer 
 *   values are used as the quality value</td><td>true</td>
 *  </tr>
 * </table>
 * All parameters are optional and can be omitted. In this case, the default value is chosen.
 * Note, if a mapping results in a value not within 0 and 100,
 * then a clipping is applied choosing the value 0 or 100 being closest to the mapped
 * value.
 * <br/>
 * <br/>
 * For example, OFIQ's configuration for the background uniformity measure looks as follows
 * <pre>
 * {
 *   "config" :
 *   ...
 *   "params": {
 *      ...
 *      "BackgroundUniformity": {
 *        "Sigmoid" : {
 *          "h": 190,
 *          "a": 1,
 *          "s": -1,
 *          "x0": 10,
 *          "w" : 100,
 *          "round": true
 *        }
 *      }
 *      ...
 *   }
 * }
 * </pre>
 * 
 * @section sec_api C++ API
 * To use OFIQ in a C++ application one needs to include the following header file.
 * <pre>
 * #include <ofiq_lib.h>
 * </pre>
 * In the following, we assume that the namespace \link OFIQ OFIQ\endlink and \link OFIQ_LIB OFIQ_LIB\endlink are used.
 * <pre>
 * using namespace OFIQ;
 * using namespace OFIQ_LIB;
 * </pre>
 * An OFIQ instance is initialized using 
 * the \link OFIQ::Interface Interface\endlink class as follows.
 * <pre>
 *   // Get implementation pointer
 *   auto implPtr = Interface::getImplementation();
 *   // Initialization
 *   auto ret = implPtr->initialize(configDir,configFile);
 * </pre>
 * Here <code>configDir</code> is a <code>std::string</code>-representation of the path to the 
 * directory in which a JAXN configuration file of name <code>configFile</code> 
 * is stored - as documented in the @ref sec_config "configuration section". Note, that the path
 * can be absolute or relative to the path of the current working directory.
 * <br />
 * <br />
 * The input image is read by using
 * the \link OFIQ_LIB::readImage readImage\endlink function as follows
 * <pre>
 *   Image image;
 *   ReturnStatus retStatus = readImage(imagePath, image);
 * </pre>
 * where <code>imagePath</code> is a <code>std::string</code>-representation of a path to
 * an image file. The representation is written to the <code>image</code> object of
 * type \link OFIQ::Image Image\endlink.
 * <br/>
 * <br/>
 * To compute the quality assessments, run
 * <pre>
 * FaceImageQualityAssessment assessment;
 * ReturnStatus retStatus = implPtr->vectorQuality(image, assessment);
 * </pre>
 * A successful computation is indicated by <code>retStatus.code</code> if it is of value 
 * \link OFIQ::ReturnCode::Success ReturnCode::Success\endlink. Then the assessment result is stored in a 
 * \link OFIQ::FaceImageQualityAssessment FaceImageQualityAssessment\endlink struct object. The obtained
 * \link OFIQ::FaceImageQualityAssessment FaceImageQualityAssessment\endlink object
 * has a <code>std::map</code> member which, for a specified \link OFIQ::QualityMeasure QualityMeasure\endlink
 * key, returns the \link OFIQ::QualityMeasureResult QualityMeasureResult\endlink. A
 * \link OFIQ::QualityMeasureResult QualityMeasureResult\endlink struct object contains the native quality
 * score stored in the \link OFIQ::QualityMeasureResult::rawScore rawScore\endlink member and the
 * quality component value stored in the \link OFIQ::QualityMeasureResult::scalar scalar\endlink
 * member. Note, both members are encoded as a <code>double</code> values although the 
 * \link OFIQ::QualityMeasureResult::scalar scalar\endlink member should 
 * (on successful quality measure computation) be an integer value between 0 and 100. To check whether
 * a \link OFIQ::QualityMeasureResult QualityMeasureResult\endlink has been computed successfully, one
 * checks if its \link OFIQ::QualityMeasureResult::code code\endlink member agrees with the value
 * \link OFIQ::QualityMeasureReturnCode::Success QualityMeasureReturnCode::Success\endlink.
 * 
 * @section sec_workflow Implementation and pre-processing workflow
 * Quality assessment is controlled by the implementation of 
 * the \link OFIQ_LIB::OFIQImpl OFIQImpl\endlink class. A shared pointer to an
 * \link OFIQ_LIB::OFIQImpl OFIQImpl\endlink object is returned by the 
 * \link OFIQ::Interface::getImplementation() Interface::getImplementation()\endlink
 * function. The implementation needs to be initialized once using the
 * \link OFIQ_LIB::OFIQImpl::initialize() OFIQImpl::initialize()\endlink function. Note, the 
 * \link OFIQ_LIB::OFIQImpl::initialize() OFIQImpl::initialize()\endlink function 
 * loads all model files as specified in the input configuration into memory; thus,
 * one should avoid creating repeated instances of the \link OFIQ_LIB::OFIQImpl OFIQImpl\endlink.
 * 
 * After successful initialization, the implementation object can be used and one can
 * repeatedly invoke the 
 * \link OFIQ_LIB::OFIQImpl::vectorQuality() OFIQImpl::vectorQuality()\endlink function
 * to assess the quality of a series of facial images.
 * 
 * The internal workflow of the \link OFIQ_LIB::OFIQImpl::vectorQuality() OFIQImpl::vectorQuality()\endlink
 * implementation is as follows.
 * <ol>
 *  <li>
 *   Pre-processing of the input image using the \link OFIQ_LIB::OFIQImpl::performPreprocessing() OFIQImpl::performPreprocessing()\endlink function.
 *   <ol>
 *    <li>Face detection implemented by \link OFIQ_LIB::modules::detectors::SSDFaceDetector::UpdateFaces() SSDFaceDetector::UpdateFaces()\endlink</li>.
 *    <li>Pose estimation implemented by \link OFIQ_LIB::modules::poseEstimators::HeadPose3DDFAV2::updatePose() HeadPose3DDFAV2::updatePose()\endlink</li>.
 *    <li>Landmark extraction implemented by \link OFIQ_LIB::modules::landmarks::ADNetFaceLandmarkExtractor::updateLandmarks() ADNetFaceLandmarkExtractor::updateLandmarks()\endlink</li>.
 *    <li>Facial alignment implemented by \link OFIQ_LIB::OFIQImpl::alignFaceImage() OFIQImpl::alignFaceImage()\endlink.
 *    <li>Face parsing implemented by \link OFIQ_LIB::modules::segmentations::FaceParsing::UpdateMask() FaceParsing::UpdateMask()\endlink.
 *    <li>Face occlusion segmentation implemented by \link OFIQ_LIB::modules::segmentations::FaceOcclusionSegmentation::UpdateMask() FaceOcclusionSegmentation::UpdateMask()\endlink.
 *   </ol>
 *  </li>
 *  <li>
 *   Quality assessment using the \link OFIQ_LIB::modules::measures::Executor::ExecuteAll() Executor::ExecuteAll()\endlink function:
 *   For all requested measures
 *   <ol>
 *    <li>its \link OFIQ_LIB::modules::measures::Measure::Execute() Execute()\endlink
 *    <li>and then its  \link OFIQ_LIB::modules::measures::Measure::SetQualityMeasure() SetQualityMeasure()\endlink functions are invoked.
 *   </ol>
 *  </li>
 * </ol>
 * 
 * @section sec_release_notes Release notes
 * This is OFIQ @ref sec_version_1_0_0_rc. 
 * The following table lists all measures and its implementation provided by this release of OFIQ. Details on the 
 * configuration and on requesting measures can be found
 * @ref sec_default_config "here". Note, the QAA identifiers listed in the table are defined in ISO/IEC 29794-5.
 * <table>
 *  <tr>
 *   <td><b>QAA identifier</b></td>
 *   <td><b>Description</b></td>
 *   <td><b>OFIQ implementation reference</b></td>
 *  </tr>
 *
 *  <tr>
 *   <td>0x41</td>
 *   <td>[MagFace](https://github.com/IrvingMeng/MagFace)-based unified quality score measure.</td>
 *   <td>\link OFIQ_LIB::modules::measures::UnifiedQualityScore UnifiedQualityScore\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x42</td>
 *   <td>Gradient-based background uniformity.</td>
 *   <td>\link OFIQ_LIB::modules::measures::BackgroundUniformity BackgroundUniformity\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x43</td>
 *   <td>Illumination unformity by summing up the minima of the histograms of the left and the right side of the face.</td>
 *   <td>\link OFIQ_LIB::modules::measures::IlluminationUniformity IlluminationUniformity\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x44</td>
 *   <td>Luminance mean measure computed from the luminance histogram</td>
 *   <td>\link OFIQ_LIB::modules::measures::Luminance Luminance\endlink</td>
 *  </tr>
 *
 *  <tr>
 *   <td>0x45</td>
 *   <td>Luminance variance measure computed from the luminance histogram</td>
 *   <td>\link OFIQ_LIB::modules::measures::Luminance Luminance\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x46</td>
 *   <td>Under-exposure prevention by computing the proportion
 *   of low-intensity pixels in the luminance image to assess the abscence of under-exposure</td>
 *   <td>\link OFIQ_LIB::modules::measures::UnderExposurePrevention UnderExposurePrevention\endlink</td>
 *  </tr>
 *
 *  <tr>
 *   <td>0x47</td>
 *   <td>Over-exposure prevention by computing the proportion
 *   of high-intensity pixels in the luminance image to assess the abscence of over-exposur</td>
 *   <td>\link OFIQ_LIB::modules::measures::OverExposurePrevention OverExposurePrevention\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x48</td>
 *   <td>Dynamic range computed from the luminance histogram.</td>
 *   <td>\link OFIQ_LIB::modules::measures::DynamicRange DynamicRange\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x49</td>
 *   <td>Sharpness assessment based on a random forest classifier trained by the 
 *   OFIQ development team.</td>
 *   <td>\link OFIQ_LIB::modules::measures::Sharpness Sharpness\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x4A</td>
 *   <td>Assessment of the absence of compression artifact (both JPEG and JPEG2000) based on a 
 *   CNN trained by the OFIQ development team.</td>
 *   <td>\link OFIQ_LIB::modules::measures::CompressionArtifacts CompressionArtifacts\endlink</td>
 *  </tr>
 * 
  *  <tr>
 *   <td>0x4B</td>
 *   <td>Assessment of the naturalness of the colour based on the conversion of the 
 *   RGB presentation of the image to the CIELAB colour space.</td>
 *   <td>\link OFIQ_LIB::modules::measures::NaturalColour NaturalColour\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x4C</td>
 *   <td>Assessment of the uniqueness of the most dominant face detected by comparing its size 
 *   with the size of the second largest face detected</td>
 *   <td>\link OFIQ_LIB::modules::measures::SingleFacePresent SingleFacePresent\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x4D</td>
 *   <td>Eyes openness assessment based on computing eyes aspect ratio from eye landmarks</td>
 *   <td>\link OFIQ_LIB::modules::measures::EyesOpen EyesOpen\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x4E</td>
 *   <td>Mouth closed assessment based on computing a ratio from mouth landmarks</td>
 *   <td>\link OFIQ_LIB::modules::measures::MouthClosed MouthClosed\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x4F</td>
 *   <td>Eyes visibility assessment by measuring the coverage of the eye visibility zone with
 *   the result of face occlusion segmentation computed during pre-processing.</td>
 *   <td>\link OFIQ_LIB::modules::measures::EyesVisible EyesVisible\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x50</td>
 *   <td>Assessment of the absence of mouth occlusion by measuring the coverage of the mouth region
 *   from mouth landmarks with the result of face occlusion segmentation computed on pre-processing.</td>
 *   <td>\link OFIQ_LIB::modules::measures::MouthOcclusionPrevention MouthOcclusionPrevention\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x51</td>
 *   <td>Assessment of the absence of face occlusion by measuring the coverage of the landmarked region
 *   with the result of face occlusion segmentation computed during pre-processing.</td>
 *   <td>\link OFIQ_LIB::modules::measures::FaceOcclusionPrevention FaceOcclusionPrevention\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x52</td>
 *   <td>Inter-eye distance assessment based on computing the Euclidean length of eyes' centres
 *   and multiplication with the secant of the yaw angle computed during pre-processing.</td>
 *   <td>\link OFIQ_LIB::modules::measures::InterEyeDistance InterEyeDistance\endlink</td>
 *  </tr>
 *
 *  <tr>
 *   <td>0x53</td>
 *   <td>Size of the head based on computing the height of the face computed from facial landmarks
 *   with the height of the image.</td>
 *   <td>\link OFIQ_LIB::modules::measures::HeadSize HeadSize\endlink</td>
 *  </tr>
 *
 *  <tr>
 *   <td>0x54</td>
 *   <td>Leftward crop of the face image</td>
 *   <td>\link OFIQ_LIB::modules::measures::CropOfTheFaceImage CropOfTheFaceImage\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x55</td>
 *   <td>Rightward crop of the face image</td>
 *   <td>\link OFIQ_LIB::modules::measures::CropOfTheFaceImage CropOfTheFaceImage\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x56</td>
 *   <td>Downward crop of the face image</td>
 *   <td>\link OFIQ_LIB::modules::measures::CropOfTheFaceImage CropOfTheFaceImage\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x57</td>
 *   <td>Upward crop of the face image</td>
 *   <td>\link OFIQ_LIB::modules::measures::CropOfTheFaceImage CropOfTheFaceImage\endlink</td>
 *  </tr>
 *  
 *  <tr>
 *   <td>0x58</td>
 *   <td>Pose angle yaw frontal alignment based on the [3DDFAV2](https://github.com/cleardusk/3DDFA_V2).</td>
 *   <td>\link OFIQ_LIB::modules::measures::HeadPose HeadPose\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x59</td>
 *   <td>Pose angle pitch frontal alignment based on the [3DDFAV2](https://github.com/cleardusk/3DDFA_V2)</td>
 *   <td>\link OFIQ_LIB::modules::measures::HeadPose HeadPose\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x5A</td>
 *   <td>Pose angle roll frontal alignment based on the [3DDFAV2](https://github.com/cleardusk/3DDFA_V2)</td>
 *   <td>\link OFIQ_LIB::modules::measures::HeadPose HeadPose\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x5B</td>
 *   <td>Expression neutrality estimation based on a fusion of [HSEMotion](https://github.com/HSE-asavchenko/face-emotion-recognition) with 
 *   with [Efficient-Expression-Neutrality-Estimation](https://github.com/dasec/Efficient-Expression-Neutrality-Estimation).</td>
 *   <td>\link OFIQ_LIB::modules::measures::ExpressionNeutrality ExpressionNeutrality\endlink</td>
 *  </tr>
 * 
 *  <tr>
 *   <td>0x5C</td>
 *   <td>Assessment of the absence of head coverings by counting the pixels being labeled 
 *   as head covers in the mask output by the face parsing computed during pre-processing.</td>
 *   <td>\link OFIQ_LIB::modules::measures::NoHeadCoverings NoHeadCoverings\endlink</td>
 *  </tr>
 * 
 * </table>
 * 
 * @subsection sec_changelog Changelog
 * @subsubsection sec_version_1_0_0_rc Version 1.0.0-RC (2024-03-15)
 * Initial release of OFIQ's release candidate providing the measures described in the @ref sec_release_notes
 */
#pragma once