import cv2
from ofiq import OFIQ, OFIQError

# ----------------------------------------------------------------------
# Example usage
# ----------------------------------------------------------------------
def test_ofiq(config_dir, config_file, img_path):
    img = None
    ofiq_inst = None
    try:
        # Print library version
        print("OFIQ version:", OFIQ.get_version())

        # Initialise the library
        ofiq_inst = OFIQ(config_dir, config_file)

        # Load an image
        # ofiq_img = OFIQ.read_image(img_path)
        # img = _image_to_numpy(ofiq_img)  # convert to numpy for easier handling

        img = cv2.imread(img_path)

        print("Starting scalar_quality()")
        # Scalar quality
        scalar = ofiq_inst.scalar_quality(img)
        print("Scalar quality:", scalar)

        print("Starting vector_quality()")
        # Vector quality
        vec = ofiq_inst.vector_quality(img)
        for r in vec:
            print(
                f"Measure {OFIQ.quality_measure_name(r['measure'])}: "
                f"scalar={r['scalar']:.2f}, native={r['rawScore']:.2f}"
            )

        print("Starting vector_quality_ext()")
        # Extended vector quality
        ext = ofiq_inst.vector_quality_ext(img)
        print("Detected faces:", len(ext["faces"]))
        print("Landmarks of biggest face:", len(ext["landmarks"]))
        print("segmentation mask:", ext["segmentationMask"].shape)
        print("occlusion mask:", ext["occlusionMask"].shape)
        print("landmarked region mask:", ext["landmarkedRegionMask"].shape)

    except OFIQError as e:
        print("OFIQ error:", e)
    finally:
        if ofiq_inst:
            del ofiq_inst  # ensure cleanup
            print("OFIQ instance released.")


if __name__ == "__main__":
    cfg_dir = "data"
    cfg_file = "ofiq_config.jaxn"
    img_path = r"data/tests/images/b-01-smile.png"

    test_ofiq(cfg_dir, cfg_file, img_path)
