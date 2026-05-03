"""Top-level package for OFIQ Server API"""

__author__ = """Maxim Schaubert"""
__email__ = 'maxim.schaubert@secunet.com'
__version__ = '0.1.0'
__all__ = ['OFIQAPI']

import logging
from numpy import ndarray
from typing import List, Dict, Any

from ofiq import OFIQ, OFIQError

logger = logging.getLogger('OFIQServerAPI')

class OFIQAPI():
    def __init__(self, config_dir=None, config_file=None):

        self.cfg_dir = None
        self.cfg_file = None

        if config_dir is not None and config_file is not None:
            self.initialize(config_dir, config_file)


    def initialize(self, config_dir, config_file):
        """Initialize the OFIQ module using a config file."""

        logger.debug(f"Initializing OFIQ using config file: {config_file} in directory: {config_dir}")

        self.cfg_dir = config_dir
        self.cfg_file = config_file

        try:
            self.ofiq = OFIQ(self.cfg_dir, self.cfg_file)
        except OFIQError as e:
            logger.error(f"Initialization failed: {e})")
            return False, str(e)

        return True, "OK"

    def scalar_quality(self, image: ndarray) -> int:
        """Perform scalar quality assessment on the given image."""

        scalar_score = self.ofiq.scalar_quality(image)

        return int(scalar_score)

    def vector_quality(self, image: ndarray) -> List[Dict[str, Any]]:
        """Perform vector quality assessment on the given image."""

        qa_results = self.ofiq.vector_quality(image)

        # replace measure ids with measure names
        for res in qa_results:
            res["measure"] = self.ofiq.quality_measure_name(res["measure"])

        return qa_results

    def face_image_quality(self, image: ndarray) -> Dict[str, Any]:
        """Perform vector quality assessment on the given image."""

        qa_results = self.ofiq.vector_quality_ext(image)

        # replace measure ids with measure names
        for res in qa_results["quality"]:
            res["measure"] = self.ofiq.quality_measure_name(res["measure"])

        return qa_results
