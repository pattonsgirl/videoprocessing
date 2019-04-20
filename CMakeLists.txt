cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
#need to set gcc verison
set (CMAKE_CXX_STANDARD 11)
#global project title?
set (projName "VidVis")
project(${projName})

#must set directory for v7.1.1
set(VTK_DIR "/home/kduncan/VTK-Release-build")


find_package(VTK REQUIRED)
find_package(OpenCV REQUIRED)

if (NOT VTK_FOUND)
  message("Skipping Compile: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
#we should only be working in if statement, since we are using old verison
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(${projName} MACOSX_BUNDLE ${projName}.cxx)
  target_link_libraries(${projName} PRIVATE ${OpenCV_LIBS} ${VTK_LIBRARIES})

else ()
  # include all components
  add_executable(${projName} MACOSX_BUNDLE ${projName}.cxx )
  target_link_libraries(${projName} PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${projName}
    MODULES ${VTK_LIBRARIES}
    )
endif ()
