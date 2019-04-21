# videoprocessing
## CEG7450 (Cyber Security Visualization)

### Goal: 
Create video processing solution for traffic feeds.
Use background subtraction & other methods to clean up image (eliminate noise)

### To Run:
* I hard coded my VTK library in CMakeLists.txt
* May need to modify on other machines\
`<mkdir build>`\
In build directory:\
`<cmake ..>`\
`<make>`\
`<./VidVis>`

### Tools:
* VTK 7.1.1
* OpenCV 4.1

### Coding Notes:
#### For cleaning up images:
* OpenCV 4.1 has MOG, MOG2, and KNN for background subtraction
* Intial exploration is with KNN background subtraction
* Looked into despeckle filters - added fastNlMeansDenoising - ran after KNN background subtraction
  * Would need fastNlMeansDenoisingColored if processing original image
* Explored blurring to reduce pixel noise.  Did not notice improvements with medianBlur / GaussianBlur
* In the end, stuck with MOG2 with custom parameters and stopped working on external methods.
* Future work: would explore connected components algorithms
#### For volume rendering:
* Pursued applying vector of actors.  Image type cannot be converted in similar way.
  * Committed code in case anyone wants to refer to old version
* Best VTK example is ReadDICOMSeries
  * Takes a series of images and stacks them
* Another lead - VTK Image Stack
  * Is not overlapping image frames in way I thought it was.
  * Also, don't know how to load all frames into buffer to display as one volume



### Notes on files:
* vastp1.mov & vastp2.mov are files provided for analysis
* Used code from InteractWithImage to get functions to translate from 
  * OpenCV matrix to VTK pipleline & breadboard of VTK pipeline


### Examples referenced:
* Lecture Slides for Image Processing / Feature Detection
  * http://avida.cs.wright.edu/courses/CEG7560/CEG7560_8.pdf
* Background Subtraction:
  * https://docs.opencv.org/3.4/d1/dc5/tutorial_background_subtraction.html
* Values (parameters) for MOG2 besides default:
  * https://docs.opencv.org/2.4/modules/video/doc/motion_analysis_and_object_tracking.html?highlight=backgroundsubtractor#backgroundsubtractor
* Denoising:
  * https://docs.opencv.org/4.1.0/d1/d79/group__photo__denoise.html#ga03aa4189fc3e31dafd638d90de335617
* Blurring:
  * https://docs.opencv.org/4.1.0/d4/d86/group__imgproc__filter.html#ga564869aa33e58769b4469101aac458f9
* Python examples (used to find classes in C for blurring)
  * https://docs.opencv.org/3.1.0/d4/d13/tutorial_py_filtering.html
* Flipping images in OpenCV:
  * Oldschool:
  * https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_imgproc/py_geometric_transformations/py_geometric_transformations.html
  * Newschool:
  * http://amin-ahmadi.com/2017/06/01/how-to-rotate-andor-flip-mirror-images-in-opencv/
* Creating array of OpenCV Mats:
  * https://stackoverflow.com/questions/9138537/opencv-creating-an-array-of-mat-objects
* Pursued applying vector of actors - no go
  * https://lorensen.github.io/VTKExamples/site/Cxx/Visualization/VectorOfActors/


### Not implemented, but for reference of others:
* Connected Components Algorithm(s) - to be added in combination of background subtraction
  * https://github.com/opencv/opencv/blob/master/samples/cpp/connected_components.cpp
  * https://docs.opencv.org/4.1.0/d3/dc0/group__imgproc__shape.html

