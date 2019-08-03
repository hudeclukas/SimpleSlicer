cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0071 NEW)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

project(SimpleSlicer)

set(IS_SRC "${CMAKE_CURRENT_SOURCE_DIR}")
set(OpenCV_DIR "D:/OpenCV/OpenCV-build" CACHE PATH "OpenCV directory override" FORCE)
set(Qt5_DIR "C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5")
set(CUDA_TOOLKIT_ROOT_DIR "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v9.0" CACHE PATH "Cuda v9 directory override v8" FORCE)

find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)

add_executable(SimpleSlicer 
  SlicerMain.cpp
  SlicerGUI.cpp
  OpencvViewer.cpp
  DirIO.cpp
  Histology.cpp

  SlicerGUI.h
  OpencvViewer.h
  DirIO.h
  Converter.h
  Histology.h

  SimpleGUI.ui

  ${WIN_RESOURCE_FILE}
)

include_directories( 
  ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(SimpleSlicer 
  ${OpenCV_LIBS}
  Qt5::Widgets
  Qt5::Gui
  Qt5::Core
)

add_custom_command(
  TARGET SimpleSlicer POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:Qt5::Widgets>
	$<TARGET_FILE:Qt5::Core>
	$<TARGET_FILE:Qt5::Gui>
	$<TARGET_FILE_DIR:SimpleSlicer>
)

add_custom_command(
  TARGET SimpleSlicer POST_BUILD
  COMMAND 
	    ${CMAKE_COMMAND} -E copy_if_different
		${OpenCV_DIR}/bin/Release/opencv_ximgproc346.dll
		${OpenCV_DIR}/bin/Release/opencv_imgcodecs346.dll
        ${OpenCV_DIR}/bin/Release/opencv_highgui346.dll
        ${OpenCV_DIR}/bin/Release/opencv_videoio346.dll
		${OpenCV_DIR}/bin/Release/opencv_imgproc346.dll
		${OpenCV_DIR}/bin/Release/opencv_core346.dll
		${OpenCV_DIR}/bin/Release/opencv_calib3d346.dll
		${OpenCV_DIR}/bin/Release/opencv_features2d346.dll
		${OpenCV_DIR}/bin/Release/opencv_flann346.dll
		$<TARGET_FILE_DIR:SimpleSlicer>
)

 add_custom_command(
   TARGET SimpleSlicer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
 		${OpenCV_DIR}/bin/Debug/opencv_ximgproc346d.dll
 		${OpenCV_DIR}/bin/Debug/opencv_imgcodecs346d.dll
        ${OpenCV_DIR}/bin/Debug/opencv_highgui346d.dll
        ${OpenCV_DIR}/bin/Debug/opencv_videoio346d.dll
 		${OpenCV_DIR}/bin/Debug/opencv_imgproc346d.dll
 		${OpenCV_DIR}/bin/Debug/opencv_core346d.dll
 		${OpenCV_DIR}/bin/Debug/opencv_calib3d346d.dll
 		${OpenCV_DIR}/bin/Debug/opencv_features2d346d.dll
 		${OpenCV_DIR}/bin/Debug/opencv_flann346d.dll
 		$<TARGET_FILE_DIR:SimpleSlicer>
 )
