cmake_minimum_required(VERSION 3.27.4)
project(HandDetection VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} main.cpp)
    target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})
else ()
    message(FATAL_ERROR "opencv not found")
endif()
