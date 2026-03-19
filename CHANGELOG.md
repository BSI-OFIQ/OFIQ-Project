# Changes

## Version 1.1.2 (2026-02-05)
- In v1.0.3 using ```OFIQ::Image::deepcopy```, the pixel data was interpreted as RGB. In v1.1.0 and v1.1.1 the pixel data has been interpreted as BGR which is a change in the API. To patch this, an argument ```bool isRgb``` has been added to the argument list of ```OFIQ::Image::deepcopy``` and which is ```true``` by default. When ```true``` the method ```OFIQ::Image::deepcopy``` interprets the
specified data as RGB; otherwise, if ```false```, ```OFIQ::Image::deepcopy``` interprets the specified pixel data as BGR.
- In v1.1.0 a new return code ```OFIQ::ReturnCode::MissingConfigFileError``` has been inserted in the enum. Consequently, integer casts of v1.1.0 and v1.1.1 were not compatible with integer casts of v1.0.3. In v1.1.2, the return code has been moved to the end of the enum so that integer casts of return codes from v1.0.3 match with integers casts of return codes from v1.1.2.
- Fixes compilation without conan for linux-arm64.
- Fixes of typos in documentation.

## Version 1.1.1 (2026-01-21)
- Updates URL to external library dependency hosted by eu-LISA
- Adds a CONTRIBUTING.md
- OFIQSampleApp labels native scores clearly by a .native prefix

## Version 1.1.0 (2025-10-31)
- Some runtime improvements have been implemented that affect conformance tests. In this way, the resulting OFIQ version is
a conformant implementation by means of ISO/IEC 29794-5. However, it is not a reference implementation. Therfore, the minor version
has been increased. The latest reference implementation is OFIQ v1.0.3.
- Provides compilation scripts and demo apps for Android and iOS
- Adds android asset-manager support (cf. https://github.com/BSI-OFIQ/OFIQ-Project/pull/70)
- Links with libjpeg-turbo (previously, libjpeg was used) when compiled with Conan
- Documentation that OFIQ uses openjpeg by default for JPEG2000 support
- OFIQ can now be safely accessed by multiple threads
- Switches building documentation from Conan 2.0.17 to 2.18.1
- Fixes building script so that it does not download onnxruntime when the flag --no-download is specified

## Version 1.0.3 (2025-07-04)

- Added a new interface method ```vectorQualityWithPreprocessingResults``` that works exactly as the existing ```vectorQuality``` method, but additionally returns image preprocessing results. The preprocessing result types are defined in the  struct ```PreprocessingResultType``` and include detected faces, face landmark points, face parsing segmentation mask, face occlusion mask and landmarked region mask.
- Includes a list of known issues
- Accepts https://github.com/BSI-OFIQ/OFIQ-Project/pull/71: Removes unneeded includes to opencv/highgui.cpp
- Accepts https://github.com/BSI-OFIQ/OFIQ-Project/pull/69: Fixes type of Image.data
- Fixes https://github.com/BSI-OFIQ/OFIQ-Project/issues/65: OFIQSampleApp crashed when specified with inconsistent arguments

## Version 1.0.2 (2025-04-10)

- Supports compilation for Raspberry Pi/ARMv8 - see https://github.com/BSI-OFIQ/OFIQ-Project/pull/45
- Fixes segmentation faults occuring on natural colour assessment in cases CIELAB values being attempted to be computed from an empty colour image
- ```EyesVisible``` component did not handle the cases where inter-eye distance is not computable. Now it does by resulting in ```FailureToAssess```.
- ```IlluminationUniformity``` did not handle the case when histograms from empty subimages are to be computed. Now it does by resulting in ```FailureToAssess```.
- ```FailureToAssess``` assessments of quality component do output a quality component value of -1 more consequently.

## Version 1.0.1 (2025-03-11)

- ```OFIQ::Image``` struct had no method to import deep copies from binary data.
   This made it unpractical to use OFIQ with a binding such as Java/JNI. Now ```OFIQ::Images``` provides a method ```OFIQ::Image::deepcopy``` for importing deep copies from binary data.

   This change resolves https://github.com/BSI-OFIQ/OFIQ-Project/issues/63.
- Code quality improvements as per Sonaqube checks (do not affect conformance tests)
- Memory management refinements
- Fixes memory allocations when ```cv::copyMakeBorder``` from OpenCV is invoked

## Version 1.0.0 (2025-03-07)

First OFIQ release being the reference implementation for ISO/IEC 29794-5.

## Version 1.0.0-RC.2 (2024-07-31)

Second release of OFIQ's release candidate. The following changes have been implemented.

- Supports compilation on MacOS
- Successfully tested that it is possible (with some effort) to compile for and be conformant with mobile devices such as Android and iOS.
- Revisions as per up coming FDIS (e.g., quality mappings, update of conformance test table, etc.)
- Fix of the default config file: Changes to the CropOfTheFaceImage measures did not affect the quality mappings
- Fixes link to the OFIQ-MODELS.zip archive: https://github.com/BSI-OFIQ/OFIQ-Project/issues/12
- Removes libgtk dependency: https://github.com/BSI-OFIQ/OFIQ-Project/issues/18
- Removes Lapack from ubuntu cmake file which wasn't used: https://github.com/BSI-OFIQ/OFIQ-Project/issues/20
- Improves readability of source code by applying a style guide to member variables: https://github.com/BSI-OFIQ/OFIQ-Project/issues/27
- Fixes from static code analyses, for example:
  - https://github.com/BSI-OFIQ/OFIQ-Project/issues/28
  - https://github.com/BSI-OFIQ/OFIQ-Project/issues/29
  - https://github.com/BSI-OFIQ/OFIQ-Project/issues/30
  - https://github.com/BSI-OFIQ/OFIQ-Project/issues/31
  - and others
- Removes mis-leading usage hint in the sample app: https://github.com/BSI-OFIQ/OFIQ-Project/issues/34
- Avoids redundant RGB conversions: https://github.com/BSI-OFIQ/OFIQ-Project/issues/36
- Fixes a bug on continuous OpenCV matrices: https://github.com/BSI-OFIQ/OFIQ-Project/issues/41
- Fixes further issues and bugs and code beautification

## Version 1.0.0-RC.1 (2024-03-15)

Initial release of OFIQ's release candidate.

