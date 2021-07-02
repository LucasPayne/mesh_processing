cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 17)
project(mesh_processing)

include_directories(../../lib)

add_library(mesh_processing
    surface_mesh.cpp
)
target_compile_options(mesh_processing PRIVATE -Wall -g)
