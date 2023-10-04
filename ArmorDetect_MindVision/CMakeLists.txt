cmake_minimum_required(VERSION 3.16)
project(drone)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory("./" PROJECT_SOURCES)
add_executable(drone ${PROJECT_SOURCES})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
link_directories("/usr/local/lib/")


include_directories(sensors)
add_subdirectory(sensors)
target_link_libraries(drone sensors ${OpenCV_LIBS})






