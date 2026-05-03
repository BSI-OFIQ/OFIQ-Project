# OFIQ Python wrapper

The OFIQ Python package is a Python wrapper around the OFIQ C API

## Install

To build the OFIQ project with python bindings the argument `--build-python` should be used:

```bash
cd ofiq_project_root/scripts
./build.sh --build-python
```

In the final steps, the build script generates a Python bindings package `ofiq` and two archives: one source distribution archive, e.g. `ofiq-1.2.0.tar.gz` and a Python wheel package, e.g. `ofiq-1.2.0-py3-none-any.whl`. The exact file names can be seen in the terminal output:

```bash
...
Building ofiq (1.2.0)
Building sdist
Building wheel
'dist/ofiq-1.2.0-py3-none-any.whl' -> '../install_x86_64_linux/Release/ofiq-1.2.0-py3-none-any.whl'
'dist/ofiq-1.2.0.tar.gz' -> '../install_x86_64_linux/Release/ofiq-1.2.0.tar.gz'
Building finished
```

It is also possible to bild the python package separately, after the ofiq project was already built. To do so, run the following commands from the project root directory:

```bash
# copy required ofiq libs to python ofiq package
cp install_x86_64_linux/Release/lib/* ./python/ofiq/

# navigate to python package folder and create a fresh virtual environment
cd python
python -m venv .venv
source .venv/bin/activate
pip install poetry

# build the package
poetry build --clean

# copy created Python wheel package to the install directory
cp -pv dist/*  ../install_x86_64_linux/Release/
```

Finally, the `ofiq` package can be installed from the Python wheel with the following command from the root folder of repository:

Linux:

```bash
pip install ./install_x86_64_linux/Release/ofiq-1.2.0-py3-none-any.whl
```

## OFIQ C API

The full specification of the C API methods can be found in [OFIQlib/include/ofiq_libc.h](../OFIQlib/include/ofiq_libc.h). Here we list the methods that have corresponding wrappers in the Python implementation:

```cpp
// Initializes the OFIQ library with the specified configuration file.
OFIQ_C_EXPORT ReturnStatus
    OFIQInitialize(OFIQHandle* handle, const char* configDir, const char* configFileName);

// Cleans up resources initialized by OFIQInitialize().
OFIQ_C_EXPORT ReturnStatus
    OFIQCleanup(OFIQHandle* handle);

// Reads an image from the specified file into the provided Image object.
OFIQ_C_EXPORT ReturnStatus
    OFIQReadImage(const char* filename, Image& image);

// Frees the memory allocated for the specified Image object.
OFIQ_C_EXPORT ReturnStatus
    OFIQFreeImage(Image* image);

// This function computes a scalar image quality measure for the given image.
OFIQ_C_EXPORT ReturnStatus
    OFIQScalarQuality(OFIQHandle handle, const Image& face,
    double& quality);

// Performs a face image quality assessment using various quality measurement methods
OFIQ_C_EXPORT ReturnStatus
    OFIQVectorQuality(OFIQHandle handle, const Image& faceImage,
    FaceImageQualityAssessment& qualityAssessment);

/** Performs a face image quality assessment by using various quality measurement methods.
 * Additionally to the OFIQVectorQuality method this function returns preprocessing results such as
 * - bounding boxes of all faces detected in the processed image
 * - face landmarks of the biggest (most prominent) face
 * - face masks and face segmentation images
 */
OFIQ_C_EXPORT ReturnStatus
    OFIQVectorQualityExt(OFIQHandle handle, const Image& faceImage,
    FaceImageQualityAssessment& qualityAssessment,
    PreprocessingResult& preprocessingResult);

// Retrieves the version information of the OFIQ library.
OFIQ_C_EXPORT void OFIQGetVersion(int& major, int& minor, int& patch);

// Gets the name of a quality measure as a string.
OFIQ_C_EXPORT const char* OFIQGetQualityMeasureName(QualityMeasure measure);
```

## Python wrapper

The full documentation of the Python wrapper for OFIQ can be found in [ofiq_python.md](ofiq_python.md)

### Testing

To test the OFIQ python wrapper run the following commands from the root folder of repository:

```bash
# create a python virtual environment
python -m venv .ofiqenv

# activate it
source .ofiqenv/bin/activate

# install ofiq python wrapper
pip install ./install_x86_64_linux/Release/ofiq-1.2.0-py3-none-any.whl

# simple test if the ofiq module can be imported. this shouldn't produce any errors
python -c "import ofiq;"

# run the test script that imports ofiq module, initialises an OFIQ instance
# and calls the API methods scalar_quality(), vector_quality() and vector_quality_ext()
# using a test image
python ./python/test/test.py
```

The output of the test script should look like this:

```text
OFIQ version: (1, 1, 2)
Starting scalar_quality()
Scalar quality: 65.0
Starting vector_quality()
Measure UnifiedQualityScore: scalar=65.00, native=24.61
Measure BackgroundUniformity: scalar=95.00, native=9.90
Measure IlluminationUniformity: scalar=72.00, native=0.33
Measure LuminanceMean: scalar=98.00, native=0.41
Measure LuminanceVariance: scalar=65.00, native=0.06
Measure UnderExposurePrevention: scalar=100.00, native=0.11
Measure OverExposurePrevention: scalar=100.00, native=0.00
Measure DynamicRange: scalar=96.00, native=7.66
Measure Sharpness: scalar=100.00, native=50.00
Measure NoCompressionArtifacts: scalar=100.00, native=0.94
Measure NaturalColour: scalar=100.00, native=0.00
Measure SingleFacePresent: scalar=100.00, native=0.00
Measure EyesOpen: scalar=100.00, native=0.11
Measure MouthClosed: scalar=94.00, native=0.04
Measure EyesVisible: scalar=100.00, native=0.00
Measure MouthOcclusionPrevention: scalar=100.00, native=0.00
Measure FaceOcclusionPrevention: scalar=98.00, native=0.02
Measure InterEyeDistance: scalar=35.00, native=57.40
Measure HeadSize: scalar=2.00, native=0.23
Measure LeftwardCropOfFace: scalar=100.00, native=4.85
Measure RightwardCropOfFace: scalar=100.00, native=2.94
Measure MarginAboveTheFace: scalar=58.00, native=1.43
Measure MarginBelowTheFace: scalar=100.00, native=2.96
Measure HeadPoseYawFrontal: scalar=85.00, native=22.48
Measure HeadPosePitchFrontal: scalar=99.00, native=5.51
Measure HeadPoseRollFrontal: scalar=100.00, native=1.96
Measure ExpressionNeutrality: scalar=95.00, native=9456.73
Measure NoHeadCoverings: scalar=100.00, native=0.00
Starting vector_quality_ext()
Detected faces: 1
Landmarks of biggest face: 98
segmentation mask: (450, 360)
```

## Web service

The web service requires a config file with some important settings. We provide a default config file [service/server/ofiq_server_config.yaml](service/server/ofiq_server_config.yaml) with the following settings:

```yaml
server:
  port: 8088
  # the level is one of the predefined levels (CRITICAL, ERROR, WARNING, INFO, DEBUG)
  loglevel: WARNING
ofiq:
  # relative path from the server directory to the data directory in repository root folder,
  # which contains the config file and other resources
  data_dir: ../../../data
  cfg_file: ofiq_config.jaxn
```

You could modify those parameters according to your environment, e.g. set other server port number or other logging level.

The server implementation is located in the following folder:

```bash
cd python/service/server/
```

Here you can also find the server configuration file. To pass the config file to the server we use the environment variable
`OFIQ_SRV_CFG`. It can be set in the terminal session using the command:

```bash
export OFIQ_SRV_CFG=ofiq_server_config.yaml
```

or it could be set before the server start:

```bash
OFIQ_SRV_CFG=ofiq_server_config.yaml <your command>
```

The server implementation requires some python packages, to install them run this command from the server folder (`python/service/server/`):

```bash
pip install -r requirements.txt
```

To start the server there are two options: using the [Connexion](https://github.com/zalando/connexion) library or using a production ready [gunicorn](https://gunicorn.org/) server.

### Connexion library

 The first option is to use the [Connexion](https://github.com/zalando/connexion) library on top of Flask, both packages are installed after you run the `pip install ...` shown above. This option is intended to be used only for development. To run the server use the following command (don't forget to set the environmant variable `OFIQ_SRV_CFG` before the call):

```bash
python -m ofiq_server
```

The output should look like this:

```bash
`ConnexionMiddleware.run` is optimized for development. For production, run using a dedicated ASGI server.
INFO:     Started server process [1559804]
INFO:     Waiting for application startup.
INFO:     Application startup complete.
INFO:     Uvicorn running on http://127.0.0.1:8088 (Press CTRL+C to quit)
```

### Gunicorn

The second option is to use `gunicorn` as a dedicated ASGI server (is installed after you run the `pip install ...`). To run the server use the following command:

```bash
gunicorn ofiq_api_server:app --bind 0.0.0.0:8088 --worker-class uvicorn.workers.UvicornWorker
```

The output should look like this:

```bash
[2026-03-03 13:16:33 +0200] [1560327] [INFO] Starting gunicorn 25.3.0
[2026-03-03 13:16:33 +0200] [1560327] [INFO] Listening at: http://0.0.0.0:8088 (1560327)
[2026-03-03 13:16:33 +0200] [1560327] [INFO] Using worker: uvicorn.workers.UvicornWorker
[2026-03-03 13:16:33 +0200] [1560334] [INFO] Booting worker with pid: 1560334
[2026-03-03 13:16:33 +0200] [1560327] [INFO] Control socket listening at /run/user/1000/gunicorn.ctl
[2026-03-03 13:16:51 +0200] [1560334] [INFO] Started server process [1560334]
[2026-03-03 13:16:51 +0200] [1560334] [INFO] Waiting for application startup.
[2026-03-03 13:16:51 +0200] [1560334] [INFO] Application startup complete.
```

### Web service API

The web service was implemented according the OFIQ API defined in [service/server/ofiq_server/swagger/swagger.yaml](service/server/ofiq_server/swagger/swagger.yaml). The API is defined using the OpenAPI Specification v3.0.3 and is stored in yaml format. It is possible to generate client implementations in different programming languages and frameworks using this API specification. You can try [Swagger Codegen](https://github.com/swagger-api/swagger-codegen) or [OpenAPI Generator](https://github.com/OpenAPITools/openapi-generator) to generate a client in your favourite language.

We provide a client implementation generated using [Swagger Codegen](https://github.com/swagger-api/swagger-codegen) and integrated into a [streamlit](https://streamlit.io/) web application.

## Client web application

The client requires a running server instance, so don't forget to start the web service before running the client web application.

The client implementation is located in the following folder:

```bash
cd python/service/client/
```

The client implementation requires some python packages, to install them run this command from the client folder:

```bash
pip install -r requirements.txt
```

The client application requires two parameters to be set in environment variables before the start, e.g.:

```bash
export OFIQAPI_HOST=http://localhost:8088/
export OFIQAPI_LOGLEVEL=DEBUG
```

To run the client (a streamlit application) run the following command:

```bash
streamlit run app.py
```

After that you should see the following output:

```bash
2026-03-03 13:33:08.703 Uvicorn server started on 0.0.0.0:8501

  You can now view your Streamlit app in your browser.

  Local URL: http://localhost:8501
  Network URL: http://172.18.122.121:8501
```

The web interface should look like this:

![Streamlit web UI](screenshot_web_ui.png)