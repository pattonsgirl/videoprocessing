#include <vtkSmartPointer.h>
#include <vtkOBJReader.h>
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
//#include <vtkSphereSource.h>
//#include <vtkGlyph3D.h>
//#include <vtkGraph.h>
//#include <vtkGraphToPolyData.h>
//includes from IneractWithImage
#include <vtkJPEGReader.h>
#include <vtkImageMapper3D.h>
#include <vtkImageActor.h> // Note: this is a 3D actor (c.f. vtkImageMapper which is 2D)
#include <vtkInteractorStyleImage.h>
#include <vtkImageImport.h>
#include <vtkImageData.h>
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

//functions from InteractWithImage
//converts from opencv matrix to VTK matrix for VTK output
void fromMat2Vtk( Mat src, vtkImageData* dest ) {
  vtkImageImport *importer = vtkImageImport::New();
  Mat frame;
  cvtColor( src, frame, COLOR_BGR2RGB);
  if (dest) { dest = importer->GetOutput(); }
  importer->SetDataSpacing( 1, 1, 1 );
  importer->SetDataOrigin( 0, 0, 0 );
  importer->SetWholeExtent( 0, frame.cols - 1, 0, frame.rows - 1, 0, 0 );
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents (frame.channels());
  importer->SetImportVoidPointer( frame.data );
  importer->Update();
}

vtkImageData* fromMat2Vtk( Mat src ) {
  vtkImageImport *importer = vtkImageImport::New();
  static Mat frame;
  cvtColor( src, frame, cv::COLOR_BGR2RGB);
  importer->SetDataSpacing( 1, 1, 1 );
  importer->SetDataOrigin( 0, 0, 0 );
  importer->SetWholeExtent( 0, frame.cols - 1, 0, frame.rows - 1, 0, 0 );
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents (frame.channels());
  importer->SetImportVoidPointer( frame.data );
  importer->Update();
  return importer->GetOutput();
}

//int main(int, char* [])
int main(int argc, char* argv[])
{

  //read in video file
  string inputFilename = "../vastp1.mov";
  //string inputFilename = "../InteractWithImage/Bunny.jpg";

  //try to open video file, else throw error
  VideoCapture capture(inputFilename);
  if (!capture.isOpened()){
      //error in opening the video input
      cerr << "Unable to open: " << inputFilename << endl;
      return 0;
  }
  

  //Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorKNN();
  Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2();

  //Ptr<BackgroundSubtractor> pBackSub = BackgroundSubtractor();

  
  //TODO::need to set video capture to mat format

  Mat frame, fgMask, denoise, blur, combo;
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
    
    

    //get the input from the keyboard
    int keyboard = waitKey(30);
    if (keyboard == 'q' || keyboard == 27)
      break;
  }
  

  //apply VTK translation in video loop?
  // Create an actor
  vtkImageActor *actor = vtkImageActor::New();
  //actor->GetMapper()->SetInputConnection(reader->GetOutputPort());
  actor->GetMapper()->SetInputData(fromMat2Vtk (fgMask));

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
