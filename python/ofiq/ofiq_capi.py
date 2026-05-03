# --------------------------------------------------------------
# ofiq_capi.py – a Python wrapper around the OFIQ C library
# --------------------------------------------------------------

import ctypes
from ctypes import (
    addressof,
    c_int,
    c_uint8,
    c_uint16,
    c_int16,
    c_double,
    c_char_p,
    c_void_p,
    POINTER,
    c_size_t,
    Structure,
    byref,
)
import os
import sys
from enum import Enum
import numpy as np

# ------------------------------------------------------------------
# Loading of the shared library
# ------------------------------------------------------------------
# Adjust the name / path according to your platform:

_lib_name = {
    "win32": "ofiq_libc.dll",
    "darwin": "libofiq.dylib",
    "linux": "libofiq_c.so",
}[sys.platform.split('_')[0]]

ofiq_dirname = os.path.dirname(__file__)
# print(f"Looking for {_lib_name} in {ofiq_dirname}...")
_lib_path = lib_path = os.path.join(ofiq_dirname, _lib_name)
_ofiq = ctypes.CDLL(_lib_path)


# ------------------------------------------------------------------
# Enumerations (mirrored as simple integer constants)
# ------------------------------------------------------------------
# Return codes
class ReturnCode(Enum):
    Success = 0
    ImageReadingError = 1
    ImageWritingError = 2
    MissingConfigFileError = 3
    MissingConfigParamError = 4
    UnknownConfigParamError = 5
    FaceDetectionError = 6
    FaceLandmarkExtractionError = 7
    FaceOcclusionSegmentationError = 8
    FaceParsingError = 9
    UnknownError = 10
    QualityAssessmentError = 11
    NotImplemented = 12

# Quality measure ids (mirrored as simple integer constants)
class QualityMeasure:
    UnifiedQualityScore = 0x41
    BackgroundUniformity = 0x42
    IlluminationUniformity = 0x43
    Luminance = -0x44
    LuminanceMean = 0x44
    LuminanceVariance = 0x45
    UnderExposurePrevention = 0x46
    OverExposurePrevention = 0x47
    DynamicRange = 0x48
    Sharpness = 0x49
    CompressionArtifacts = 0x4a
    NaturalColour = 0x4b
    SingleFacePresent = 0x4c
    EyesOpen = 0x4d
    MouthClosed = 0x4e
    EyesVisible = 0x4f
    MouthOcclusionPrevention = 0x50
    FaceOcclusionPrevention = 0x51
    InterEyeDistance = 0x52
    HeadSize = 0x53
    CropOfTheFaceImage = -0x54
    LeftwardCropOfTheFaceImage = 0x54
    RightwardCropOfTheFaceImage = 0x55
    MarginAboveOfTheFaceImage = 0x56
    MarginBelowOfTheFaceImage = 0x57
    HeadPose = -0x58
    HeadPoseYaw = 0x58
    HeadPosePitch = 0x59
    HeadPoseRoll = 0x5a
    ExpressionNeutrality = 0x5b
    NoHeadCoverings = 0x5c
    NotSet = -1


class FaceDetectorType:
    OPENCVSSD = 0
    NotSet = 1


class LandmarkType:
    LM_98 = 0
    NotSet = 1


class QualityMeasureReturnCode:
    Success = 0
    FailureToAssess = 1
    NotInitialized = 2


# ------------------------------------------------------------------
# C structs (exact layout as in the header)
# ------------------------------------------------------------------
class OFIQHandle(c_void_p):
    """Opaque pointer returned by OFIQInitialize."""
    pass

class Image(Structure):
    _fields_ = [
        ("width", c_uint16),
        ("height", c_uint16),
        ("depth", c_uint8),  # 8 or 24
        ("data", POINTER(c_uint8)),  # owned buffer – free with OFIQFreeImage
    ]


class BoundingBox(Structure):
    _fields_ = [
        ("xleft", c_int16),
        ("ytop", c_int16),
        ("width", c_int16),
        ("height", c_int16),
        ("faceDetector", c_int),  # FaceDetectorType (int)
    ]


class LandmarkPoint(Structure):
    _fields_ = [("x", c_int16), ("y", c_int16)]


class FaceLandmarks(Structure):
    _fields_ = [
        ("type", c_int),  # LandmarkType
        ("landmarks", POINTER(LandmarkPoint)),
        ("numLandmarks", c_int),
    ]


class QualityMeasureResult(Structure):
    _fields_ = [
        ("measure", c_int),  # QualityMeasure
        ("rawScore", c_double),
        ("scalar", c_double),
        ("code", c_int),  # QualityMeasureReturnCode
    ]


class FaceImageQualityAssessment(Structure):
    _fields_ = [
        ("qAssessments", POINTER(QualityMeasureResult)),
        ("numAssessments", c_size_t),
        ("boundingBox", BoundingBox),
    ]


class PreprocessingResult(Structure):
    _fields_ = [
        ("faces", POINTER(BoundingBox)),
        ("numFaces", c_size_t),
        ("landmarks", FaceLandmarks),
        ("segmentationMask", Image),
        ("occlusionMask", Image),
        ("landmarkedRegionMask", Image),
    ]

class OFIQError(RuntimeError):
    pass

# ------------------------------------------------------------------
# Helper functions for converting between C structs and Python data structures
# ------------------------------------------------------------------

def _image_to_numpy(img):
    """
    internal function to convert an Image struct (with data owned by the C library)
    into a numpy array without copying the pixel data.
    The returned array shares memory with the original C buffer,
    so it should not be modified and will become invalid
    once OFIQFreeImage is called on the original Image struct.

    :param img: An Image struct containing the image data and dimensions.
    :type img: Image
    """
    channels = 3 if img.depth == 24 else 1

    nbytes = img.width * img.height * channels
    addr = addressof(img.data.contents)

    buf_type = (c_uint8 * nbytes)
    buf = buf_type.from_address(addr)

    arr = np.frombuffer(buf, dtype=np.uint8)

    if channels == 1:
        arr = arr.reshape((img.height, img.width))
    else:
        arr = arr.reshape((img.height, img.width, 3))

    return arr


def _numpy_to_image(np_arr: np.ndarray) -> Image:
    """
    Internal function to convert a numpy array (H x W or H x W x 3, dtype uint8)
    to an Image struct with a copy of pixel data.

    :param arr: A numpy array containing the image data (grayscale or RGB, dtype uint8).
    :type arr: np.ndarray
    :return: An Image struct containing the image data and dimensions,
        with a copy of the pixel data.
    :rtype: Image
    """
    arr_c = np.ascontiguousarray(np_arr, dtype=np.uint8)

    height, width = arr_c.shape[:2]
    depth = 24 if arr_c.ndim == 3 and arr_c.shape[2] == 3 else 8

    nbytes = width * height * (3 if depth == 24 else 1)

    img = Image()
    img.width  = c_uint16(width)
    img.height = c_uint16(height)
    img.depth  = c_uint8(depth)
    img.data = (c_uint8 * nbytes)()
    ctypes.memmove(img.data, arr_c.ctypes.data, nbytes)

    return img

# ------------------------------------------------------------------
# Function prototypes
# ------------------------------------------------------------------

def _proto(func_name, argtypes, restype=None):
    func = getattr(_ofiq, func_name)
    func.argtypes = argtypes
    func.restype = restype
    return func


# OFIQInitialize
OFIQInitialize = _proto(
    "OFIQInitialize",
    [POINTER(OFIQHandle), c_char_p, c_char_p],
    c_int,
)

# OFIQCleanup
OFIQCleanup = _proto("OFIQCleanup", [POINTER(OFIQHandle)], c_int)

# Image I/O
OFIQReadImage = _proto(
    "OFIQReadImage",
    [c_char_p, POINTER(Image)],
    c_int,
)

OFIQReadImageFromByteArray = _proto(
    "OFIQReadImageFromByteArray",
    [POINTER(c_uint8), c_size_t, POINTER(Image)],
    c_int,
)

OFIQFreeImage = _proto("OFIQFreeImage", [POINTER(Image)], c_int)

# Quality assessment (scalar)
OFIQScalarQuality = _proto(
    "OFIQScalarQuality",
    [OFIQHandle, POINTER(Image), POINTER(c_double)],
    c_int,
)

# Vector quality
OFIQVectorQuality = _proto(
    "OFIQVectorQuality",
    [OFIQHandle, POINTER(Image), POINTER(FaceImageQualityAssessment)],
    c_int,
)

# Extended vector quality
OFIQVectorQualityExt = _proto(
    "OFIQVectorQualityExt",
    [
        OFIQHandle,
        POINTER(Image),
        POINTER(FaceImageQualityAssessment),
        POINTER(PreprocessingResult),
    ],
    c_int,
)

# Free helpers
OFIQFreeFaceImageQualityAssessment = _proto(
    "OFIQFreeFaceImageQualityAssessment",
    [POINTER(FaceImageQualityAssessment)],
    c_int,
)

OFIQFreeFaceLandmarks = _proto(
    "OFIQFreeFaceLandmarks",
    [POINTER(FaceLandmarks)],
    c_int,
)

OFIQFreePreprocessingResult = _proto(
    "OFIQFreePreprocessingResult",
    [POINTER(PreprocessingResult)],
    c_int,
)

# Version & measure name helpers
OFIQGetVersion = _proto(
    "OFIQGetVersion",
    [POINTER(c_int), POINTER(c_int), POINTER(c_int)],
    None,
)

OFIQGetQualityMeasureName = _proto(
    "OFIQGetQualityMeasureName",
    [c_int],
    c_char_p,
)


# ------------------------------------------------------------------
# 6. High‑level Python wrapper class
# ------------------------------------------------------------------
class OFIQ:
    """Python wrapper around the OFIQ C API."""

    def __init__(self, config_dir: str, config_file: str):
        """
        Initialize the OFIQ library with a configuration directory and file.

        :param config_dir: A path to a directory containing OFIQ configuration data
        :type config_dir: str
        :param config_file: A name of a configutaion file (e.g., "ofiq_config.jaxn") located within the config_dir
        :type config_file: str
        """
        self.handle = OFIQHandle(0)
        initRes = OFIQInitialize(
            byref(self.handle), config_dir.encode(), config_file.encode())
        if ReturnCode(initRes) != ReturnCode.Success:
            raise OFIQError(ReturnCode(initRes))

    def free(self):
        """
        Release resources associated with the OFIQ handle. This should be called when done with the OFIQ instance.
        """
        if getattr(self, "handle", None):
            OFIQCleanup(byref(self.handle))
            self.handle = None

    def __del__(self):
        self.free()

    @staticmethod
    def get_version():
        """
        Return the version of the OFIQ library as a tuple (major, minor, patch).
        """
        major = c_int()
        minor = c_int()
        patch = c_int()
        OFIQGetVersion(byref(major), byref(minor), byref(patch))
        return major.value, minor.value, patch.value

    @staticmethod
    def quality_measure_name(measure: int) -> str:
        """
        Return the name of a quality measure given its integer code.

        :param measure: Integer code of the quality measure (e.g., QualityMeasure.UnifiedQualityScore)
        :type measure: int
        :return: Name of the quality measure, or "unknown" if not recognized
        :rtype: str
        """
        name_ptr = OFIQGetQualityMeasureName(measure)
        return name_ptr.decode() if name_ptr else "unknown"

    # ------------------------------------------------------------------
    # Image handling
    # ------------------------------------------------------------------
    @staticmethod
    def read_image(path: str) -> Image:
        """
        Read an image from the specified file path and return an Image struct.

        :param path: Path to the image file to read.
        :type path: str
        :return: An Image struct containing the image data and dimensions.
        :rtype: Image
        """
        img = Image()
        OFIQReadImage(path.encode(), byref(img))
        return img

    @staticmethod
    def free_image(img: Image):
        """
        Release the memory associated with an Image struct that was returned by read_image().

        :param img: An Image struct whose data buffer was allocated by the C library and should be freed.
        :type img: Image
        """
        OFIQFreeImage(byref(img))

    # ------------------------------------------------------------------
    # Quality assessment (scalar)
    # ------------------------------------------------------------------
    def scalar_quality(self, img: np.ndarray) -> float:
        """
        Assess the quality of a face image using the scalar quality measure.

        :param img: A numpy array containing the image data (grayscale or RGB, dtype uint8).
        :type img: np.ndarray
        :return: The scalar quality score as a float in the range [0.0, 100.0], where higher is better.
        :rtype: float
        """
        img_struct = _numpy_to_image(img)
        quality = c_double()
        OFIQScalarQuality(self.handle, img_struct, byref(quality))
        return quality.value

    # ------------------------------------------------------------------
    # Quality assessment (vector)
    # ------------------------------------------------------------------
    def vector_quality(self, img: np.ndarray):
        """
        Assess the quality of a face image using various quality measurement methods.
        Returns a list of dictionaries, each containing the measure id, raw score, scalar score, and return code.

        :param img: A numpy array containing the image data (grayscale or RGB, dtype uint8).
        :type img: np.ndarray
        """
        img_struct = _numpy_to_image(img)
        qa = FaceImageQualityAssessment()
        OFIQVectorQuality(self.handle, img_struct, byref(qa))

        # Convert the C array of QualityMeasureResult into a Python list
        results = []
        for i in range(qa.numAssessments):
            res = qa.qAssessments[i]
            results.append(
                {
                    "measure": res.measure,
                    "rawScore": res.rawScore,
                    "scalar": res.scalar,
                    "code": res.code,
                }
            )
        # Remember to free the native allocation when done
        OFIQFreeFaceImageQualityAssessment(byref(qa))
        return results

    # ------------------------------------------------------------------
    # Extended vector quality (including preprocessing results)
    # ------------------------------------------------------------------
    def vector_quality_ext(self, img: np.ndarray):
        """
        Assess the quality of a face image using various quality measurement methods,
        also returning preprocessing results such as detected faces, landmarks, and masks.

        :param img: A numpy array containing the image data (grayscale or RGB, dtype uint8).
        :type img: np.ndarray
        """
        img_struct = _numpy_to_image(img)
        qa = FaceImageQualityAssessment()
        pre = PreprocessingResult()
        OFIQVectorQualityExt(self.handle, img_struct, byref(qa), byref(pre))

        # ------- quality results (same conversion as above) -------
        quality_results = []
        for i in range(qa.numAssessments):
            r = qa.qAssessments[i]
            quality_results.append(
                {
                    "measure": r.measure,
                    "rawScore": r.rawScore,
                    "scalar": r.scalar,
                    "code": r.code,
                }
            )

        # ------- preprocessing results (example extraction) -------
        faces = []
        for i in range(pre.numFaces):
            b = pre.faces[i]
            faces.append(
                {
                    "x": b.xleft,
                    "y": b.ytop,
                    "w": b.width,
                    "h": b.height,
                    "detector": b.faceDetector,
                }
            )

        # landmarks (copy into Python list)
        landmarks = []
        if pre.landmarks.landmarks:
            for i in range(pre.landmarks.numLandmarks):
                pt = pre.landmarks.landmarks[i]
                landmarks.append((pt.x, pt.y))

        # copy data
        segmentation_mask = _image_to_numpy(pre.segmentationMask).copy()
        occlusion_mask    = _image_to_numpy(pre.occlusionMask).copy()
        region_mask       = _image_to_numpy(pre.landmarkedRegionMask).copy()

        # Clean up native allocations
        OFIQFreeFaceImageQualityAssessment(byref(qa))
        OFIQFreePreprocessingResult(byref(pre))

        return {
            "quality": quality_results,
            "faces": faces,
            "landmarks": landmarks,
            "segmentationMask": segmentation_mask,
            "occlusionMask": occlusion_mask,
            "landmarkedRegionMask": region_mask,
        }
