# coding: utf-8

import sys
from setuptools import setup, find_packages

NAME = "ofiq_server"
VERSION = "1.0.0"
# To install the library, run the following
#
# python setup.py install
#
# prerequisite: setuptools
# http://pypi.python.org/pypi/setuptools

REQUIRES = [
    "connexion",
    "swagger-ui-bundle>=0.0.2"
]

setup(
    name=NAME,
    version=VERSION,
    description="OFIQ API",
    author_email="",
    url="",
    keywords=["Swagger", "OFIQ API"],
    install_requires=REQUIRES,
    packages=find_packages(),
    package_data={'': ['swagger/swagger.yaml']},
    include_package_data=True,
    entry_points={
        'console_scripts': ['ofiq_server=ofiq_server.__main__:main']},
    long_description="""\
    API for perfoming requests against the Open Face Image Quality service.
    """
)
