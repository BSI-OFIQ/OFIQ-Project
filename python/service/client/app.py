""" _summary_
    This module contains the streamlit web app for the OFIQ API server.
"""
from __future__ import print_function
import os
import logging
import base64
import binascii
from io import BytesIO
import streamlit as st
from PIL import Image, ImageDraw
import numpy as np
import pandas as pd
import ofiq_client
from ofiq_client.rest import ApiException
from ofiq_client import (
    Configuration,
    AnalysisRequestData,
    AnalysisResponseData,
    FaceImageAnalysisResult,
    QualityAssessmentResults,
    QualityAssessmentResult,
    BoundingBox,
    Landmarks,
    Image as OFIQImage
)

logging.getLogger('PIL').setLevel(logging.WARNING)


def base64_to_pil_image(base64_data):
    """Helper function to decode base64 data into a PIL image."""

    decoded_bytes = None
    try:
        decoded_bytes = base64.b64decode(base64_data)
    except binascii.Error as e:
        print("Error decoding base64 data:", str(e))
        return None

    # Wrap in BytesIO and open as PIL Image
    img = Image.open(BytesIO(decoded_bytes)).convert('RGB')
    return img

def apply_mask_to_image(image: Image.Image,
                        mask: Image.Image | np.ndarray,
                        crop_rect: tuple[int, int, int, int] | None) -> Image.Image:
    """Apply mask to image

    :param image: Image to be masked
    :type image: Image.Image
    :param mask: Image mask to be applied. Can be a PIL Image or a numpy array.
        If the mask has multiple channels, only the first channel will be used.
    :type mask: Image.Image | np.ndarray
    :param crop_rect: If specified, only apply the mask to the cropped region
        defined by the rectangle (x, y, width, height).
    :type crop_rect: tuple[int, int, int, int] | None
    :return: Masked image
    :rtype: Image.Image
    """
    if isinstance(mask, np.ndarray):
        np_mask = mask
    else:
        np_mask: np.ndarray = np.array(mask)

    # conver RGB(A) mask to grayscale if needed. just take the first channel
    if len(np_mask.shape) == 3:
        np_mask = np_mask[:,:,0]

    if np_mask.max() == 1:
        np_mask = np_mask * 255

    if crop_rect is not None:
        image_cropped = image.crop(crop_rect)
        mask_cropped = Image.fromarray(np_mask).crop(crop_rect)
        return Image.composite(
            image_cropped,
            Image.new("RGB", image_cropped.size, (0,0,0)),
            mask_cropped)

    return Image.composite(
        image,
        Image.new("RGB", image.size, (0,0,0)),
        Image.fromarray(np_mask))

def draw_segmentation(segments: Image.Image) -> Image.Image:
    """Draw face segmentation regions on a given image. The segmentation mask
        is expected to have pixel values of 0 for background and >0 for
        different face regions.

    :param segments: Description
    :type segments: Image.Image
    :return: Description
    :rtype: Image
    """
    np_segm: np.ndarray = np.array(segments)

    # conver RGB(A) mask to grayscale if needed. just take the first channel
    if len(np_segm.shape) == 3:
        np_segm = np_segm[:,:,0]

    # segments with ID > 25 are not relevant for visualization, set them to 0
    np_segm[np_segm > 25] = 0
    # for better visualization, multiply the segment IDs with a factor
    # (e.g. 20) to get more distinct colors
    np_segm = np_segm*20

    img_segments = Image.fromarray(np_segm)#.convert("RGB")
    return img_segments

def main(ofiq_host: str):

    st.sidebar.title("Face image quality analysis")

    st.sidebar.header("Upload image")
    valid_types = ['jpg', 'jpeg', 'png', 'bmp', 'JPG', 'JPEG', 'PNG', 'BMP']
    uploaded_file = st.sidebar.file_uploader(
        "Choose an image",
        type=valid_types)

    # create an instance of the API class
    if "api_instance" not in st.session_state:
        conf = Configuration()
        conf.host = ofiq_host
        st.session_state["api_instance"] = ofiq_client.DefaultApi(
            ofiq_client.ApiClient(conf))

    if uploaded_file is None:
        return

    uploaded_image = None
    try:
        uploaded_image = Image.open(uploaded_file).convert("RGB")
    except Exception as e:
        st.error("Exception %s", e)
        logging.error("Exception %s", e)
        return

    logging.debug("start processing uploaded file %s", uploaded_file.name)

    # Bild an API senden
    st.sidebar.image(uploaded_image, caption='Uploaded Image', width="content")

    uploaded_file.seek(0)

    logging.debug("encoding image data")

    image_data = base64.b64encode(uploaded_file.read()).decode('utf-8')
    # image_type = uploaded_file.type
    client_image = OFIQImage(base64_data=image_data)

    body = AnalysisRequestData(image=client_image)

    try:
        logging.debug("call face_image_quality")

        # Returns face image quality analysis results including preprocessing results
        api_response: AnalysisResponseData = \
            st.session_state["api_instance"].face_image_quality(body)

        logging.debug(api_response)

    except ApiException as e:
        print(f"Exception when calling DefaultApi->face_image_quality: {e}")
        st.error("An error occurred while performing the classification.")
        st.error("Exception %s", e)
        logging.error("Exception %s", e)
        return

    if api_response.error != 'ok':
        st.error(f"Error from server: {api_response.error}")
        logging.error(f"Error from server: {api_response.error}")
        return

    qa_result: FaceImageAnalysisResult = api_response.results

    if qa_result is None:
        st.error("No results received from the server.")
        return

    lms: Landmarks = qa_result.landmarks
    logging.debug(f"Received {len(lms.points)} landmarks.")

    landmarks = [(lp.x, lp.y) for lp in lms.points]
    logging.debug(f"Landmark points: {landmarks}")

    bbox: BoundingBox = qa_result.bounding_box
    face_crop_rect = (bbox.x, bbox.y,
                      bbox.x + bbox.width,
                      bbox.y + bbox.height)

    #
    # show preprocessing images
    #

    st.subheader("Preprocessing results")

    # draw face crop rect and landmarks on the uploaded image
    img_preproc_res = uploaded_image.copy()
    draw = ImageDraw.Draw(img_preproc_res)
    draw.rectangle(face_crop_rect, outline="red", width=2)
    for (x, y) in landmarks:
        draw.circle((x,y), radius=4, fill=(0,0,255))

    st.image(img_preproc_res,
             caption='Face bounding box and landmarks')

    img_columns = st.columns(3)

    mask_landmarked = base64_to_pil_image(
        qa_result.mask_landmarked_region.base64_data)
    if mask_landmarked is not None:
        img_masked_landmarked = apply_mask_to_image(
            uploaded_image, mask_landmarked, face_crop_rect)
        img_columns[0].image(
            img_masked_landmarked, caption='Applied landmark region mask')

    mask_occlusion = base64_to_pil_image(
        qa_result.mask_occlusion.base64_data)
    if mask_occlusion is not None:
        img_masked_occlusion = apply_mask_to_image(
            uploaded_image, mask_occlusion, face_crop_rect)
        img_columns[1].image(
            img_masked_occlusion, caption='Applied face occlusion mask')

    mask_segments = base64_to_pil_image(
        qa_result.mask_segmentation.base64_data)
    if mask_segments is not None:
        img_segments = draw_segmentation(mask_segments)
        img_columns[2].image(
            img_segments, caption='Face regions segmentation')

    #
    # show quality essessment results
    #
    st.subheader("Quality Assessment Results")
    assessments: QualityAssessmentResults = qa_result.assessments
    qa_results_list: list[QualityAssessmentResult] = assessments.results

    # sort assessments by measure name
    qa_results_list.sort(key=lambda x: x.measure)
    qa_result_table = pd.DataFrame(
        {
            "Measure": [qa.measure for qa in qa_results_list],
            "Scalar Score": [qa.scalar_score for qa in qa_results_list],
            "Native Score": [f"{qa.native_score:.4f}" for qa in qa_results_list],
        },
        index=range(1, len(qa_results_list)+1))
    st.table(qa_result_table, border="horizontal")


if __name__ == "__main__":

    cfg_host = os.environ.get('OFIQAPI_HOST', 'http://localhost:8088')
    cfg_loglevel = os.environ.get('OFIQAPI_LOGLEVEL', 'WARNING')
    logging_level = logging.getLevelName(cfg_loglevel)
    print('Logging level:', cfg_loglevel)
    logger = logging.getLogger(__name__)
    logging.basicConfig(level=logging_level)

    logging.info("Using OFIQAPI host: %s", cfg_host)

    main(cfg_host)
