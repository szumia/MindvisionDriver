cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} sensors_src)

add_library(sensors OBJECT ${sensors_src})

target_link_libraries( sensors ${OpenCV_LIBS}  /opt/MVS/lib/64/libMvCameraControl.so  "/usr/lib/libMVSDK.so")
