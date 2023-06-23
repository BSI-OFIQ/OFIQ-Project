Two files have to be downloaded manually and placed in this folder (i.e. \<OFIQ-PROJECT\>/data/models/ssd_facedetect/). These are

* *[ssd_facedetect.caffemodel](https://github.com/sr6033/face-detection-with-OpenCV-and-DNN/raw/master/res10_300x300_ssd_iter_140000.caffemodel)*
* *[ssd_facedetect.prototxt.txt](https://github.com/sr6033/face-detection-with-OpenCV-and-DNN/raw/master/deploy.prototxt.txt)*

Note, download, the names of the files may differ from the two files listed aboved. You have to manually change the file names correspondingly.

At the time of writing, these files were available from https://github.com/sr6033/face-detection-with-OpenCV-and-DNN.

On a linux like system, one may usefully download these file via the following commands:

```
$ cd <OFIQ-PROJECT>/data/models/ssd_facedetect/

$ wget https://github.com/sr6033/face-detection-with-OpenCV-and-DNN/raw/master/ res10_300x300_ssd_iter_140000.caffemodel -o ssd_facedetect.caffemodel

$ wget https://github.com/sr6033/face-detection-with-OpenCV-and-DNN/raw/master/deploy.prototxt.txt -o ssd_facedetect.prototxt.txt
```
