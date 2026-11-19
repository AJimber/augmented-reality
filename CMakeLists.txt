cmake_minimum_required(VERSION 3.10)
project(aug_real VERSION 1.7.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 4.6 REQUIRED)

add_executable(aug_real
    aug_real.cpp
    common_code.cpp
    common_code.hpp
)

target_include_directories(aug_real PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(aug_real PRIVATE ${OpenCV_LIBS})