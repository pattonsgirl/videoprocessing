#include <vtkSmartPointer.h>

#include <vtkJPEGReader.h>
#include <vtkImageMapper3D.h>
#include <vtkImageActor.h> // Note: this is a 3D actor (c.f. vtkImageMapper which is 2D)
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkTransform.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

void fromMat2Vtk( cv::Mat src,
                  vtkImageData* dest ) {
  vtkImageImport *importer =
    vtkImageImport::New();
  cv::Mat frame;
  cvtColor( src, frame, cv::COLOR_BGR2RGB);
  if (dest) { dest = importer->GetOutput(); }
  importer->SetDataSpacing( 1, 1, 1 );
  importer->SetDataOrigin( 0, 0, 0 );
  importer->SetWholeExtent( 0, frame.cols - 1, 0,
			    frame.rows - 1, 0, 0 );
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents (frame.channels());
  importer->SetImportVoidPointer( frame.data );
  importer->Update();
}

vtkImageData* fromMat2Vtk( cv::Mat src) {
  vtkImageImport *importer =
    vtkImageImport::New();
  static cv::Mat frame;
  cvtColor( src, frame, cv::COLOR_BGR2RGB);
  frame = getRotationMatrix2D((frame.cols/2,frame.rows/2),180,1)
  importer->SetDataSpacing( 1, 1, 1 );
  importer->SetDataOrigin( 0, 0, 0 );
  importer->SetWholeExtent( 0, frame.cols - 1, 0, frame.rows - 1, 0, 0 );
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents (frame.channels());
  importer->SetImportVoidPointer( frame.data );
  vtkTransform *flipper = vtkTransform::New();
  //rotate 180 - orginal was upside down
  /*flipper->RotateY(180);
  vtkImageReslice *importer_rotated = vtkImageReslice::New();
  importer_rotated->SetInputConnection(importer->GetOutputPort());
  importer_rotated->SetResliceTransform(flipper);
  //flipper->Update();
  importer_rotated->Update();
  return importer_rotated->GetOutput();*/
  importer->Update();
  return importer->GetOutput();
}

int main(int argc, char* argv[])
{
  // Parse input arguments
  if ( argc != 2 )
  {
    std::cout << "Required parameters: Filename" << std::endl;
    return EXIT_FAILURE;
  }
  std::string inputFilename = argv[1];

  cv::Mat input = cv::imread (inputFilename, cv::IMREAD_COLOR);

   // Read the image
  vtkSmartPointer<vtkJPEGReader> reader = 
      vtkSmartPointer<vtkJPEGReader>::New();
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  vtkSmartPointer<vtkImageData> otherreader =
    vtkSmartPointer<vtkImageData>::New();

  //fromMat2Vtk (input, otherreader);

  // Create an actor
  vtkSmartPointer<vtkImageActor> actor = 
    vtkSmartPointer<vtkImageActor>::New();
  //actor->GetMapper()->SetInputConnection(reader->GetOutputPort());
  actor->GetMapper()->SetInputData(fromMat2Vtk (input));

  // Setup renderer
  vtkSmartPointer<vtkNamedColors> colors =
    vtkSmartPointer<vtkNamedColors>::New();

  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  // Setup render window
  vtkSmartPointer<vtkRenderWindow> window = 
    vtkSmartPointer<vtkRenderWindow>::New();
  window->AddRenderer(renderer);

  // Setup render window interactor
  vtkSmartPointer<vtkRenderWindowInteractor> interactor = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(window);

  // Setup interactor style (this is what implements the zooming, panning and brightness adjustment functionality)
  vtkSmartPointer<vtkInteractorStyleImage> style = 
    vtkSmartPointer<vtkInteractorStyleImage>::New();
  interactor->SetInteractorStyle(style);
  
  // Render and start interaction
  interactor->Start();

  return EXIT_SUCCESS;
}
