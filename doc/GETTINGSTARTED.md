# DISCLAIMER

Currently, the scope of OFIQlib is onlay a preview featuring a subset of 
implementations of preprocessing steps and quality measures. It is very 
likely that the provided implementation will change. Do not expect backwards 
compatibility.

As of now, the preview for OFIQ features the following.
* ADNet landmark estimator
* SSD face detector
* Dynamic Range measure
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

Currently the only way to get conan working in this project is to install the version 1.59 in Python, e.g., with **pip install conan==1.59**.

To install required libraries/packages go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **01_conan-install-FaceQAlib.cmd**.

To create and build the project using cmake go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **02_build_FaceQAlib.cmd**.

To run the sample program go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **run_sample.cmd**.

To run the sample program go to **\<OFIQ-PROJECT\>\scripts\\** directory and run **run_ctest.cmd**.

## Linux

Analogous to compilation on windows, the project uses Conan and CMake on linux and features several batch scripts to guide through the building process.

In a terminal change the directory into the script folder.
```
$ cd <OFIQ-PROJECT>/scripts/
```

Install CMake first.
```
$ sudo apt-get install cmake
```

The conan package manager in the version of 1.59 should be installed first (other version may currently not work). The following commands do the job on Ubuntu 22.04.

```
$ sudo apt-get install python3-pip

$ sudo pip3 install conan==1.59.0
```

The package depencies can be install via conan using
```
$sh 01_conan-install-FaceQAlib.sh
```

To compile OFIQlib, run

```
$ sh 02_configure_and_build_FaceQAlib.cmd
```

To run the sample program, execute the following.
```
$ sh run_sample.sh
```

Likewise, to run the unittests, execute.
```
$ sh run_ctest.sh
```
