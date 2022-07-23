cmake_minimum_required(VERSION 3.16)
project(SIFT3D)

find_package(SIFT3D REQUIRED)

add_executable (main main.cpp)
target_link_libraries (main PUBLIC ${SIFT3D_LIBRARIES})
target_include_directories(main PUBLIC ${SIFT3D_INCLUDE_DIRS})