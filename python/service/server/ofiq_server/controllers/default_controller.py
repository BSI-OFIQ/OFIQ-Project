import connexion
import six

from typing import List, Dict  # noqa: F401

from ofiq_server.models.analysis_request_data import AnalysisRequestData  # noqa: E501
from ofiq_server.models.analysis_response_data import AnalysisResponseData  # noqa: E501
from ofiq_server.models.face_image_analysis_result import FaceImageAnalysisResult
from ofiq_server.models.bounding_box import BoundingBox  # noqa: F401,E501
from ofiq_server.models.image import Image  # noqa: F401,E501
from ofiq_server.models.point import Point  # noqa: F401,E501
from ofiq_server.models.landmarks import Landmarks  # noqa: F401,E501
from ofiq_server.models.quality_assessment_results import QualityAssessmentResults  # noqa: F401,E501
from ofiq_server.models.quality_assessment_result import QualityAssessmentResult  # noqa: F401,E501
from ofiq_server.models.scalar_quality_response_data import ScalarQualityResponseData  # noqa: E501
from ofiq_server.models.scalar_quality_analysis_result import ScalarQualityAnalysisResult  # noqa: F401,E501
from ofiq_server.models.vector_quality_response_data import VectorQualityResponseData  # noqa: E501
from ofiq_server.models.vector_quality_analysis_result import VectorQualityAnalysisResult  # noqa: F401,E501
from ofiq_server import util

import base64
import binascii
import logging
from io import BytesIO
from PIL import Image as PILImage
import numpy as np
import ofiq
from ofiq_server import ofiq_api_inst

SRV_LOGGER: logging.Logger = logging.getLogger('connexion.app')


def base64_to_pil_image(base64_data):
    """Helper function to decode base64 data into a PIL image."""

    decoded_bytes = None
    # Remove extraneous rubbish
    try:
        decoded_bytes = base64.b64decode(base64_data)  # .replace('\\n', '')
    except binascii.Error as e:
        print("Error decoding base64 data:", str(e))
        return None

    # Wrap in BytesIO and open as PIL Image
    img = PILImage.open(BytesIO(decoded_bytes)).convert('RGB')
    return img


def base64_to_numpy_image(base64_data):
    """Helper function to decode base64 data into a PIL image."""

    # Wrap in BytesIO and open as PIL Image
    pil_img = base64_to_pil_image(base64_data)
    if pil_img is None:
        return None
    img_rgb = np.array(pil_img)
    img_bgr = img_rgb[:,:,::-1]
    return img_bgr


def numpy_image_to_base64_png(np_img):
    """Helper function to encode a numpy image into base64 encoded PNG image."""

    if np_img.ndim == 3:
        img_rgb = np_img[:,:,::-1]
        pil_img = PILImage.fromarray(img_rgb)
    else:
        pil_img = PILImage.fromarray(np_img)

    buffered = BytesIO()
    pil_img.save(buffered, format="PNG")
    img_bytes = buffered.getvalue()
    base64_data = base64.b64encode(img_bytes).decode('utf-8')

    return base64_data

def face_image_quality(body):  # noqa: E501
    """Returns face image quality analysis results including preprocessing results

    Perform face image quality analysis, includes assessment results for face quality measurements and preprocessing results like face region bounding box, face landmarks and face region masks image quality analysis based on different face quality measurements. Returns an object that contains native and scalar scores for each image quality measurement. # noqa: E501

    :param body:
    :type body: dict | bytes

    :rtype: AnalysisResponseData
    """

    if not isinstance(body, dict):
        return AnalysisResponseData(
            error='Invalid input', results=None)

    request_data = AnalysisRequestData.from_dict(body)  # noqa: E501

    if request_data.image.base64_data is None:
        return AnalysisResponseData(
            error='image.base64_data is empty', results=None)

    np_img_bgr = base64_to_numpy_image(request_data.image.base64_data)

    if np_img_bgr is None:
        return AnalysisResponseData(
            error='Failed to read input image', results=None)

    SRV_LOGGER.debug('calling face_image_quality')

    try:
        q_assessments = ofiq_api_inst.face_image_quality(np_img_bgr)

    except ofiq.OFIQError as e:
        SRV_LOGGER.error(f'face_image_quality failed: {e}')
        return AnalysisResponseData(error=str(e), results=None)

    SRV_LOGGER.debug('face_image_quality finished successfully')

    # prepare response objects
    img_analysis_res = FaceImageAnalysisResult()

    # convert quality assessment results
    qa_results: List[QualityAssessmentResult] = []
    for assessment in q_assessments["quality"]:
        qa_res = QualityAssessmentResult(
            assessment["measure"],
            native_score=assessment["rawScore"],
            scalar_score=assessment["scalar"])
        qa_results.append(qa_res)
    img_analysis_res.assessments = QualityAssessmentResults(results=qa_results)

    # convert bounding box data
    bbox = q_assessments["faces"][0]
    img_analysis_res.bounding_box = BoundingBox(
        bbox["x"], bbox["y"], bbox["w"], bbox["h"])

    # convert landmarks data
    points: List[Point] = []
    for lm in q_assessments["landmarks"]:
        point = Point(lm[0], lm[1])
        points.append(point)
    img_analysis_res.landmarks = Landmarks(points=points)

    # convert images
    mask_segments = q_assessments["segmentationMask"]
    mask_occlusion = q_assessments["occlusionMask"] * 255
    mask_landmark_regions = q_assessments["landmarkedRegionMask"] * 255
    img_analysis_res.mask_segmentation = Image(
        base64_data=numpy_image_to_base64_png(mask_segments))
    img_analysis_res.mask_occlusion = Image(
        base64_data=numpy_image_to_base64_png(mask_occlusion))
    img_analysis_res.mask_landmarked_region = Image(
        base64_data=numpy_image_to_base64_png(mask_landmark_regions))

    response_data = AnalysisResponseData(
        error="ok"
        , results=img_analysis_res
        )
    SRV_LOGGER.debug(response_data)

    return response_data


def health():  # noqa: E501
    """Service Health Information

    Returns the current state of the service. # noqa: E501

    :rtype: None
    """
    SRV_LOGGER.debug('health called')

    return 'initialized'


def scalar_quality(body):  # noqa: E501
    """Returns an overall image quality core

    Performs an overall image quality analysis. Returns an object that contains the unified quality score for the face image provided. # noqa: E501

    :param body:
    :type body: dict | bytes

    :rtype: ScalarQualityResponseData
    """
    if not isinstance(body, dict):
        return 'Invalid input'

    request_data = AnalysisRequestData.from_dict(body)  # noqa: E501

    if request_data.image.base64_data is None:
        return 'image.base64_data is empty'

    np_img_bgr = base64_to_numpy_image(request_data.image.base64_data)

    if np_img_bgr is None:
        return AnalysisResponseData(error='Failed to read input image', results=None)

    SRV_LOGGER.debug('calling scalar_quality')

    scalar_score: int = 0
    try:
        scalar_score = ofiq_api_inst.scalar_quality(np_img_bgr)

    except ofiq.OFIQError as e:
        SRV_LOGGER.error(f'scalar_quality failed: {e}')
        return 'scalar_quality failed'

    SRV_LOGGER.debug('scalar_quality finished successfully')

    # prepare response objects
    scalar_qa_result = ScalarQualityAnalysisResult(int(scalar_score))

    response_data = ScalarQualityResponseData(
        error="ok", results=scalar_qa_result)

    SRV_LOGGER.debug(response_data)

    return response_data


def vector_quality(body):  # noqa: E501
    """Returns quality scores for different face quality measurements

    Perform image quality analysis based on different face quality measurements. Returns an object that contains native and scalar scores for each image quality measurement. # noqa: E501

    :param body:
    :type body: dict | bytes

    :rtype: VectorQualityResponseData
    """
    if not isinstance(body, dict):
        return 'Invalid input'

    request_data = AnalysisRequestData.from_dict(body)  # noqa: E501

    if request_data.image.base64_data is None:
        return 'image.base64_data is empty'

    np_img_bgr = base64_to_numpy_image(request_data.image.base64_data)

    if np_img_bgr is None:
        return AnalysisResponseData(error='Failed to read input image', results=None)

    SRV_LOGGER.debug('calling vector_quality')

    try:
        q_assessments = ofiq_api_inst.vector_quality(np_img_bgr)

    except ofiq.OFIQError as e:
        SRV_LOGGER.error(f'face_image_quality failed: {e}')
        return 'vector_quality failed'

    SRV_LOGGER.debug('vector_quality finished successfully')

    # prepare response objects
    vector_qa_results = VectorQualityAnalysisResult()

    # convert quality assessment results
    qa_results: List[QualityAssessmentResult] = []
    for assessment in q_assessments:
        qa_res = QualityAssessmentResult(
            assessment["measure"],
            native_score=assessment["rawScore"],
            scalar_score=assessment["scalar"])
        qa_results.append(qa_res)

    vector_qa_results.assessments = QualityAssessmentResults(results=qa_results)

    response_data = VectorQualityResponseData(
        error="ok", results=vector_qa_results)

    SRV_LOGGER.debug(response_data)

    return response_data
