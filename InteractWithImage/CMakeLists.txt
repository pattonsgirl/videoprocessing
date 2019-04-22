
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)
project(InteractWithImage)

#must set directory for v7.1.1
set(VTK_DIR "/home/kduncan/VTK-Release-build")

find_package(OpenCV REQUIRED)
find_package(VTK COMPONENTS 
  vtkCommonColor
  vtkCommonCore
  vtkIOImage
  vtkInteractionStyle
  vtkRenderingContextOpenGL2
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2 QUIET)
if (NOT VTK_FOUND)
  message("Skipping InteractWithImage: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE} ${OpenCV_INCLUDE_DIRS})
  add_executable(InteractWithImage MACOSX_BUNDLE InteractWithImage.cxx )
  target_link_libraries(InteractWithImage PRIVATE ${OpenCV_LIBS} ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(InteractWithImage MACOSX_BUNDLE InteractWithImage.cxx )
  target_link_libraries(InteractWithImage PRIVATE ${OpenCV_LIBS} ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS InteractWithImage
    MODULES ${VTK_LIBRARIES} ${OpenCV_LIBS} 
    )
endif () 
