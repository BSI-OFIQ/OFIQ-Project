# DISCLAIMER

Currently, OFIQlib is in an early state of developmenting featuring a subset of 
implementations of preprocessing steps and quality measures. Do not expect backwards 
compatibility.

As of now, the preview for OFIQ contains the following.
* ADNet landmark estimator
* SSD face detector
* Dynamic range measure
* Total number of faces measure
* Mouth open measure

# Compilation of OFIQlib

## Download CNN models

The model files for ADNet landmark estimator and SSD face detector have to be downloaded manually.
Details on how to obtain these files are given in [ADNet - Readme](../data/models/adnet/README.md)
and [SSD - Readme](../data/models/ssd_facedetect/README.md).

## Windows

In order to build the project using Conan and CMake several batch scripts have been set up to guide through the build.

To build the project you need to install cmake and conan

[Download and install cmake](https://cmake.org/)

[Download and install conan](https://conan.io/)

Currently, the project requires conan in the verions of 1.59 (other versions may not work). Install a working version of conan via **pip install conan==1.59**.

Install required libraries/packages: go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **01_conan-install-FaceQAlib.cmd**

Create and build the project: go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **02_build_FaceQAlib.cmd**

Run the sample program: go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **run_sample.cmd**

Run the unit tests: go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **run_ctest.cmd**

## Linux

Similar to the compilation on windows, the project uses Conan and CMake and features several batch scripts to guide through the building process.

In a terminal change the directory into the script folder.
```
$ cd <OFIQ-PROJECT>/scripts/
```

Install CMake (minimal required version is 3.22)
```
$ sudo apt-get install cmake
```

The conan package manager in the version of 1.59 should be installed (other versions may not work). The following commands do the job on Ubuntu 22.04.

```
$ sudo apt-get install python3-pip

$ sudo pip3 install conan==1.59.0
```

The required libraries and package can be install via conan using
```
$sh 01_conan-install-FaceQAlib.sh
```

Compile OFIQlib.
```
$ sh 02_configure_and_build_FaceQAlib.cmd
```

Run the sample program.
```
$ sh run_sample.sh
```

Run unit tests.
```
$ sh run_ctest.sh
```
