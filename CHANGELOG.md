### Version 1.0.2 (2025-04-10)

<ul>
 <li>Supports compilation for Raspberry Pi/ARMv8 - see https://github.com/BSI-OFIQ/OFIQ-Project/pull/45</li>
 <li>Fixes segmentation faults occuring on natural colour assessment in cases CIELAB values being attempted to be computed from an empty colour image</li>
 <li>EyesVisible component did not handle the cases where inter-eye distance is not computable. Now it does by resulting in FailureToAssess.</li>
 <li>IlluminationUniformity did not handle the case when histograms from empty subimages are to be computed. Now it does by resulting in FailureToAssess.</li>
 <li>FailureToAssess assessments of quality component do output a quality component value of -1 more consequently.</li>
</ul>

### Version 1.0.1 (2025-03-11)
 <ul>
  <li>OFIQ::Image struct had no method to import deep copies from binary data. 
   This made in unpractical to use OFIQ using a binding such as Java/JNI. Now OFIQ::Images provides
   a method \link OFIQ::Image::deepcopy deepcopy\endlink for importing deep copies from binary data.

   This change resolves https://github.com/BSI-OFIQ/OFIQ-Project/issues/63.
  </li>
  <li>
   Code quality improvements as per Sonaqube checks (do not affect conformance tests)
  </li>
  <li>
   Memory management refinements
  </li>
  <li>
   Fixes memory allocations when cv::copyMakeBorder from OpenCV is invoked
  </li>
 </ul>

### Version 1.0.0 (2025-03-07)
First OFIQ release being the reference implementation for ISO/IEC 29794-5.

### Version 1.0.0-RC.2 (2024-07-31)
Second release of OFIQ's release candidate. The following changes have been implemented.
<ul>
 <li>Supports compilation on MacOS</li>
 <li>Successfully tested that it is possible (with some effort) to compile for and be conformant with mobile devices such as Android and iOS.</li>
 <li>Revisions as per up coming FDIS (e.g., quality mappings, update of conformance test table, etc.)</li>
 <li>Fix of the default config file: Changes to the CropOfTheFaceImage measures did not affect the quality mappings</li>
 <li>Fixes link to the OFIQ-MODELS.zip archive: https://github.com/BSI-OFIQ/OFIQ-Project/issues/12</li>
 <li>Removes libgtk dependency: https://github.com/BSI-OFIQ/OFIQ-Project/issues/18</li>
 <li>Removes Lapack from ubuntu cmake file which wasn't used: https://github.com/BSI-OFIQ/OFIQ-Project/issues/20</li>
 <li>Improves readability of source code by applying a style guide to member variables: https://github.com/BSI-OFIQ/OFIQ-Project/issues/27</li>
 <li>Fixes from static code analyses, for example:
  <ul>
   <li>https://github.com/BSI-OFIQ/OFIQ-Project/issues/28</li>
   <li>https://github.com/BSI-OFIQ/OFIQ-Project/issues/29</li>
   <li>https://github.com/BSI-OFIQ/OFIQ-Project/issues/30</li>
   <li>https://github.com/BSI-OFIQ/OFIQ-Project/issues/31</li>
   <li>and others</li>
  </ul>
 </li>
 <li>Removes mis-leading usage hint in the sample app: https://github.com/BSI-OFIQ/OFIQ-Project/issues/34</li>
 <li>Avoids redundant RGB conversions: https://github.com/BSI-OFIQ/OFIQ-Project/issues/36</li>
 <li>Fixes a bug on continuous OpenCV matrices: https://github.com/BSI-OFIQ/OFIQ-Project/issues/41</li>
<li>Fixes further issues and bugs and code beautification</li>
</ul>
 
### Version 1.0.0-RC.1 (2024-03-15)
Initial release of OFIQ's release candidate.