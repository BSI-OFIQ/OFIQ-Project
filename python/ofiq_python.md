# OFIQ – Python Wrapper for the OFIQ C Library

## Table of Contents

1. [Overview](#overview)
2. [Class `OFIQ`](#class-ofiq)
   - [Constructor `__init__(config_dir, config_file)`](#init)
   - [Instance methods](#instance-methods)
     - `scalar_quality(img)`
     - `vector_quality(img)`
     - `vector_quality_ext(img)`
     - `free()`
   - [Static methods](#static-methods)
     - `get_version()`
     - `quality_measure_name(measure)`
     - `read_image(path)`
     - `free_image(img)`
3. [Helper Functions (internal)](#helpers)
4. [Enums & Constants](#enums)
5. [Typical Usage Example](#example)

---

## Overview <a name="overview"></a>

`OFIQ` is a Python wrapper around the **OFIQ** (Open Facial Image Quality) C library.
It loads the native shared library, exposes the C API through `ctypes`, and provides convenient Pythonic methods for:

* Initialising and cleaning up the OFIQ engine.
* Loading images from files.
* Computing face image quality scores as a single scalar value or as a vector of measure results of various quality components.
* Retrieving preprocessing results (detected faces, landmarks, segmentation/occlusion masks).

All heavy‑weight processing is delegated to the compiled library; the wrapper mainly handles data conversion between NumPy arrays and the C structs defined by OFIQ.

---

## Class `OFIQ` <a name="class-ofiq"></a>

```python
class OFIQ:
    """Python wrapper around the OFIQ C API."""
```

### Constructor <a name="init"></a>

```python
def __init__(self, config_dir: str, config_file: str):
```

* **Purpose** – Initialise the OFIQ library with a configuration directory and file.
* **Parameters**
  * `config_dir` – Path to the directory that contains the OFIQ configuration data.
  * `config_file` – Name of the configuration file (e.g., `ofiq_config.jaxn`).
* **Exceptions** – Raises `OFIQError` if the underlying C call returns a non‑zero `ReturnCode`.

### Instance methods <a name="instance-methods"></a>

| Method | Signature | Description |
|--------|-----------|-------------|
| **`scalar_quality(img)`** | `def scalar_quality(self, img: np.ndarray) -> float` | Assess the quality of a face image using the scalar quality measure. Returns a scalar quality score in the range [0.0, 100.0], where higher is better.|
| **`vector_quality(img)`** | `def vector_quality(self, img: np.ndarray) -> List[Dict]` | Assess the quality of a face image using various quality measurement methods. Returns a list of dictionaries, each containing the measure id, raw score, scalar score, and return code. |
| **`vector_quality_ext(img)`** | `def vector_quality_ext(self, img: np.ndarray) -> Dict` | Assess the quality of a face image using various quality measurement methods, also returning preprocessing results such as detected face bounding box, facial landmarks, and three mask images (segmentation, occlusion, landmarked region). |
| **`free()`** | `def free(self)` | Explicitly releases the OFIQ handle by calling `OFIQCleanup`. After this call `self.handle` becomes `None`. |
### Static methods <a name="static-methods"></a>

| Method | Signature | Description |
|--------|-----------|-------------|
| **`get_version()`** | `@staticmethod def get_version() -> Tuple[int, int, int]` | Returns the library version as `(major, minor, patch)`. |
| **`quality_measure_name(measure)`** | `@staticmethod def quality_measure_name(measure: int) -> str` | Translates a numeric quality‑measure ID (e.g., `QualityMeasure.UnifiedQualityScore`) to its human‑readable name. |
| **`read_image(path)`** | `@staticmethod def read_image(path: str) -> Image` | Loads an image file via the native `OFIQReadImage` and returns an `Image` struct. |
| **`free_image(img)`** | `@staticmethod def free_image(img: Image)` | Frees the memory allocated for an `Image` struct by calling `OFIQFreeImage`. |

### Cleanup helpers

Both `vector_quality` and `vector_quality_ext` free the native allocations they receive (`OFIQFreeFaceImageQualityAssessment`, `OFIQFreePreprocessingResult`) to avoid memory leaks.

---

## Helper Functions (internal) <a name="helpers"></a>

| Function | Purpose |
|----------|---------|
| `_image_to_numpy(img)` | Convert an `Image` struct (native buffer) to a NumPy array **without copying** (shares memory). Used internally when extracting mask images. |
| `_numpy_to_image(np_arr)` | Convert a NumPy array (uint8, H×W or H×W×3) to an `Image` struct with a **copy** of the pixel data. |

---

## Enums & Constants <a name="enums"></a>

The source defines several enums that are mirrored as integer constants for easy use in Python code:

* **`ReturnCode`** – Result codes from library calls (e.g., `Success = 0`, `ImageReadingError = 1`, …).
* **`QualityMeasure`** – IDs for the various face‑quality metrics (e.g., `UnifiedQualityScore = 0x41`, `Sharpness = 0x49`, `HeadPoseYaw = 0x58`, …).
* **`FaceDetectorType`**, **`LandmarkType`**, **`QualityMeasureReturnCode`** – Additional categorical values.

These can be imported from the module and used directly when interpreting results.

---

## Usage Example <a name="example"></a>

```python
import numpy as np
from ofiq_capi import OFIQ, QualityMeasure

# 1. Initialise the library
ofiq_inst = OFIQ(config_dir="/path/to/data", config_file="ofiq_config.jaxn")

# 2. Load an image (as a NumPy array)
ofiq_img = OFIQ.read_image("face.jpg")
# Convert to NumPy for processing
img_np = OFIQ._image_to_numpy(ofiq_img)   # note: internal helper; copy if you need to keep it
OFIQ.free_image(ofiq_img)                 # free native memory

# 3. Compute scalar quality
scalar_score = ofiq_inst.scalar_quality(img_np)
print(f"Scalar quality: {scalar_score:.2f}")

# 4. Get vector quality (all measures)
vector_results = ofiq_inst.vector_quality(img_np)
for r in vector_results:
    name = OFIQ.quality_measure_name(r["measure"])
    print(f"{name}: native={r['rawScore']:.3f}, scalar={r['scalar']:.2f}")

# 5. Get extended results (faces, landmarks, masks)
ext = ofiq_inst.vector_quality_ext(img_np)
print("Detected faces:", ext["faces"])
print("Landmarks (first 5):", ext["landmarks"][:5])
print("Segmentation mask:", ext["segmentationMask"].shape)
print("Occlusion mask:", ext["occlusionMask"].shape)
print("Landmarked region mask:", ext["landmarkedRegionMask"].shape)

# 6. Cleanup
ofiq_inst.free()   # or rely on destructor
```
