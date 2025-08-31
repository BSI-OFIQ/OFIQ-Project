This folder contains model files needed to run OFIQ.
The files need to be downloaded from the 
[ISO-portal](https://standards.iso.org/iso-iec/29794/-5/ed-2/en/OFIQ-MODELS.zip).
This is done automatically by the building scripts shipped with OFIQ.
The following folder structure shoulde placed directly here in this folder (not in
a sub-folder).

| File                                                                                     | Description |
|------------------------------------------------------------------------------------------|-------------|
| README.md                                                                                | The file you are reading right now |
| expression_neutrality/grimmer/hse_1_2_C_adaboost.yml.gz                                  | Adaboost classifier for expression neutrality estimation |
| [expression_neutrality/grimmer/LICENSE.md](expression_neutrality/grimmer/LICENSE.md)     | Documents the license situation of the files in expression_neutrality/grimmer/* |
| expression_neutrality/hsemotion/enet_b0_8_best_vgaf_embed_zeroed.onnx                    | CNN in ONNX format for expression neutrality estimation. |
| expression_neutrality/hsemotion/enet_b2_8_embed_zeroed.onnx                              | CNN in ONNX format for expression neutrality estimation. |
| [expression_neutrality/hsemotion/LICENSE.md](expression_neutrality/hsemotion/LICENSE.md) | Documents the license situation of the files in expression_neutrality/hsemotion/* |
| face_detection/ssd_facedetect.caffemodel                                                 | SSD model file in CAFFE format |
| face_detection/ssd_facedetect.prototxt.txt                                               | SSD's CAFFE protype file |
| [face_detection/LICENSE.md](face_detection/LICENSE.md)                                   | Documents the license situation of the files in face_detection/* |
| face_landmark_estimation/ADNet.onnx                                                      | ADNet model file in ONNX format |
| [face_landmark_estimation/LICENSE.md](face_landmark_estimation/LICENSE.md)               | Documents the license situation of the files in face_landmark_estimation/* |
| face_occlusion_segmentation/face_occlusion_segmentation_ort.onnx                         | FaceExtraction model file in ONNX format |
| [face_occlusion_segmentation/LICENSE.md](face_occlusion_segmentation/LICENSE.md)         | Documents the license situation of the files in face_occlusion_segmentation/* |
| face_parsing/bisenet_400.onnx                                                            | BiSeNet model file in ONNX format  |
| [face_parsing/LICENSE.md](face_parsing/LICENSE.md)                                       | Documents the license situation of the files in face_parsing/* |
| head_pose_estimation/mb1_120x120.onnx                                                    | 3DDFAV2 model file in ONNX format |
| [head_pose_estimation/LICENSE.md](head_pose_estimation/LICENSE.md)                       | Documents the license situation of the files in head_pose_estimation/* |
| no_compression_artifacts/ssim_248_model.onnx                                             | OFIQ's compression artifact model in ONNX format |
| [no_compression_artifacts/LICENSE.md](no_compression_artifacts/LICENSE.md)               | Documents the license situation of the files in no_compression_artifacts/* |
| sharpness/face_sharpness_rtree.xml.gz                                                    | OFIQ's random forest sharpness classifier |
| [sharpness/LICENSE.md](sharpness/LICENSE.md)                                             | Documents the license situation of the files in sharpness/* |
| unified_quality_score/magface_iresnet50_norm.onnx                                        | iResNet50 model file in ONNX format from MagFace (modified) |
| [unified_quality_score/LICENSE.md](unified_quality_score/LICENSE.md)                     | Documents the license situation of the files in unified_quality_score/* |
