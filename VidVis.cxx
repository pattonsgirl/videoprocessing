#include <vtkSmartPointer.h>
//includes from ReadOBJ
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
//includes from GeoAssignCoordinates
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkGraphMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkImageReslice.h>
//includes from IneractWithImage
#include <vtkJPEGReader.h>
#include <vtkImageMapper3D.h>
#include <vtkImageActor.h> // Note: this is a 3D actor (c.f. vtkImageMapper which is 2D)
#include <vtkInteractorStyleImage.h>
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageMapper3D.h>
#include <vtkObjectFactory.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkImageProperty.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkImageStack.h>
#include <vtkImageReader.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkImageBlend.h>
#include <vtkPointData.h>
#include <vtkUnsignedShortArray.h>

//includes for opencv
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/video/background_segm.hpp>
//#include <opencv2/bgsegm.hpp>
//includes for C++
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
using namespace cv;

//function from InteractWithImage
//converts from opencv matrix to VTK matrix for VTK output
vtkImageData* fromMat2Vtk( Mat src ) {
  vtkImageImport *importer = vtkImageImport::New();
  static Mat frame, rotMat, rotFrame;
  cvtColor( src, frame, COLOR_BGR2RGB);

  //I am proud of this and leaving it alone.
  //This chunk rotates the image 180 degrees
  //The shortcut is:
  //flip(src, dest, 0)
  Point2f f_center(frame.cols/2.0,frame.rows/2.0);
  rotMat = getRotationMatrix2D(f_center,180,1);
  Size s = frame.size();
  warpAffine(frame, rotFrame, rotMat, s);
  //still need to mirror after flipping to get original
  //orientation.  Flipped about horizontal axis
  flip(rotFrame, rotFrame, +1);

  importer->SetDataSpacing( 1, 1, 1 );
  importer->SetDataOrigin( 0, 0, 0 );
  importer->SetWholeExtent( 0, rotFrame.cols - 1, 0, rotFrame.rows - 1, 0, 0 );
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents (rotFrame.channels());
  importer->SetImportVoidPointer( rotFrame.data );

  importer->Update();
  return importer->GetOutput();
}

int main(int argc, char* argv[]){
  //read in video file
  string inputFilename = "../vastp1.mov";
  string inputJpeg = "../InteractWithImage/Bunny.jpg";

  //try to open video file, else throw error and close
  VideoCapture capture(inputFilename);
  if (!capture.isOpened()){
      //error in opening the video input
      cerr << "Unable to open: " << inputFilename << endl;
      return 0;
  }

  //MOG2 parameters stand for:
  //history=100,varThreshold=200,bShadowDetection=0
  Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2(100,200,0);

  //Intitialize some VTK stuff for use down the line
  //vtkImageData to hold images post conversion - images loaded in loop
  //vtkImageData *movieData = vtkImageData::New();
  //Create a smart volume mapper
  //vtkSmartVolumeMapper *volumeMapper = vtkSmartVolumeMapper::New();
  //volumeMapper->SetBlendModeToComposite(); 
  //in sample code, there is an image data
  //the code itself creates a sphere, slices it, and loads that into image data
  //going to try to do something similar, but put it in the cv loop instead
  //volumeMapper->SetInputData(imageData);

  
  
  Mat frame, fgMask, denoise, blur, combo;
  //array for matrices of whole video... not sure if correct direction
  vector<Mat> clean_frames;
  //process video in OpenCV, apply bg subtraction
  while (true) {
    capture >> frame;
    //check for end of video
    if (frame.empty())
      break;

    pBackSub->apply(frame, fgMask);

    //get the frame number and write it on the current frame
    rectangle(frame, Point(10, 2), Point(100,20), Scalar(255,255,255), -1);
    stringstream ss;
    ss << capture.get(CAP_PROP_POS_FRAMES);
    string frameNumberString = ss.str();
    putText(frame, frameNumberString.c_str(), Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5 , Scalar(0,0,0));
    //show the current frame and the fg masks
    imshow("Frame", frame);
    imshow("FG Mask", fgMask);
    
    //add frames / matrix to array
    clean_frames.push_back(fgMask.clone());

    //add frames to vtkImageData:
    //movieData->CopyStructure(fromMat2Vtk(fgMask));
    //movieData->Update();
    
    //get the input from the keyboard
    int keyboard = waitKey(30);
    if (keyboard == 'q' || keyboard == 27)
      break;
  }

  //let's break some stuff... 
  cv::Mat bunny = cv::imread (inputJpeg, cv::IMREAD_COLOR);

  //vtkImageActor *actor = vtkImageActor::New();
  //Works for (single) image:
  //returned type fromMat2Vtk is vtkImageData
  //with nothing additional, fgMask is the last frame read in while loop
  //frame 1 is the first frame with stuff in it, 0 should be good for getting numbers
  //actor->GetMapper()->SetInputData(fromMat2Vtk (bunny));
  vtkImageReader *img_data = vtkImageReader::New();

  //by the time we get here, we know the size of the images (how long the video is)
  vtkImageData *img_volume = vtkImageData::New();
  img_volume->SetExtent((0, frame.cols, 0, frame.rows, 0, clean_frames.size()-1));
  int numcells = img_volume->GetNumberOfCells();
  int numpoints = img_volume->GetNumberOfPoints();

  //create an array (not the volume)
  vtkUnsignedShortArray *img_array = vtkUnsignedShortArray::New();
  img_array->SetNumberOfValues(numpoints);
  //iterate through clean frames, translate to VTK (fromMat2Vtk)
  for(int i = 0; i < clean_frames.size(); i++){
    int vals = (fromMat2Vtk(clean_frames[i]))->GetPointData();
    int offset = i*clean_frames.size();
    for(int j = 0; j < vals; j++){
      img_array->SetValue(offset+j, j);
    }
  }
  
  img_volume->GetPointData()->SetArray(img_array);

  // Setup renderer
  vtkNamedColors *colors = vtkNamedColors::New();

  vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddActor(actor);
  
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  // Setup render window
  vtkRenderWindow *window = vtkRenderWindow::New();
  window->AddRenderer(renderer);

  // Setup render window interactor
  vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::New();
  interactor->SetRenderWindow(window);

  // Setup interactor style (this is what implements the zooming, panning and brightness adjustment functionality)
  vtkInteractorStyleImage *style = vtkInteractorStyleImage::New();
  interactor->SetInteractorStyle(style);
  
  // Render and start interaction
  interactor->Start();

  
  return EXIT_SUCCESS;
}
