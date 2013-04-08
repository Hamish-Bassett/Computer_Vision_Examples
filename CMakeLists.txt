cmake_minimum_required (VERSION 2.8)
project ( KinectLibCVD )

FIND_PACKAGE(OpenCV REQUIRED)

FIND_PATH(LIBCVD_INCLUDE_DIR
NAMES cvd/image_io.h cvd/video_display.h cvd/gl_helpers.h
PATHS /usr/local/include/cvd)

FIND_LIBRARY(LIBCVD_LIBS
NAMES libcvd.so
PATHS /usr/local/lib)

FIND_PATH(FREENECT_INCLUDE_DIR
NAMES libfreenect.h
PATHS /usr/local/include/libfreenect/)

FIND_LIBRARY(FREENECT_LIBS
NAMES libfreenect.so
PATHS /usr/local/lib64/libfreenect)

include_directories ("${PROJECT_SOURCE_DIR}")
#add_library(KINECTHDBAS1CVD KinectHDBAS1CVD.h)

add_executable ( KinectLibCVD KinectLibCVD.cpp )

INCLUDE_DIRECTORIES( ${FREENECT_INCLUDE_DIR})
target_link_libraries ( KinectLibCVD ${OpenCV_LIBS} ${FREENECT_LIBS} ${LIBCVD_LIBS} )
