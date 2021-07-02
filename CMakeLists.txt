cmake_minimum_required(VERSION 3.1.0)
project(mesh_processing)

set(EIGEN3_INCLUDE_DIR "usr/include/eigen3")

find_package(Eigen3 REQUIRED)

include_directories(. include ${EIGEN3_INCLUDE_DIR})

add_library(mesh_processing
    src/surface_mesh/surface_mesh.cpp
    src/surface_geometry/surface_geometry.cpp
    # Enmesh mesh generation sublibrary.
    src/enmesh/grid.cpp
    src/enmesh/io.cpp
)
target_compile_options(mesh_processing PRIVATE -Wall -g)

# Compile example applications
add_executable(simple1 examples/simple1/simple1.cpp)
target_link_libraries(simple1 mesh_processing)
