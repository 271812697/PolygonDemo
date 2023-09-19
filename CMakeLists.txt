cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(PolygonDemo VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(Source)
add_subdirectory(Extern)
