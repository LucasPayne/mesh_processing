cmake_minimum_required(VERSION 3.1.0)
project(mesh_processing)


# Compilation/extension options.
set(build_extension_assimp_convert YES)
set(ASSIMP_INCLUDE_DIR /home/lucas/computer_graphics/assimp/include) #<----Set this to the correct directory if the above is set to YES.

# External dependencies.
set(EIGEN3_INCLUDE_DIR "usr/include/eigen3")
find_package(Eigen3 REQUIRED)
add_compile_options(-DTETLIBRARY)
# add_subdirectory(dependencies/tetgen/tetgen1.6.0)
set(TETGEN_INCLUDE_DIR "dependencies/tetgen/tetgen1.6.0")

include_directories(. include ${EIGEN3_INCLUDE_DIR} ${TETGEN_INCLUDE_DIR})
link_directories(${TETGEN_INCLUDE_DIR})

add_library(mesh_processing STATIC
    # SurfaceMesh data structure.
    src/surface_mesh/surface_mesh.cpp
    src/surface_mesh/raw_editing.cpp
    src/surface_mesh/topology.cpp
    src/surface_mesh/euler_editing.cpp

    # SurfaceGeometry data structure (simple wrapper to SurfaceMesh which gives vertex positions by default).
    src/surface_geometry/surface_geometry.cpp

    # Compactified mesh/geometry data structures (intended for rendering and algorithms that don't need the flexibility of e.g. SurfaceMesh).
    src/compactified/compactified.cpp

    # Tetgen conversion.
    src/tetrahedralization/tetrahedralization.cpp
    # Compile tetgen here (---need to fix linking problems when done separately).
    dependencies/tetgen/tetgen1.6.0/tetgen.cxx
    dependencies/tetgen/tetgen1.6.0/predicates.cxx

    # Surface subdivision.
    src/subdivision/subdivision.cpp

    # Surface fairing.
    src/fairing/fairing.cpp

    # Enmesh mesh generation sublibrary.
    # src/enmesh/grid.cpp
    # src/enmesh/io.cpp
)
target_compile_options(mesh_processing PRIVATE -Wall -g)


# Build extensions.
if(build_extension_assimp_convert)
    message(STATUS "Linking extension assimp_convert")
    include_directories(${ASSIMP_INCLUDE_DIR})
    add_library(assimp_convert OBJECT
        extensions/assimp_convert/assimp_convert.cpp
    )
    set_property(TARGET assimp_convert PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(mesh_processing assimp_convert)
endif()


# Compile example applications
add_executable(simple1 examples/simple1/simple1.cpp)
target_link_libraries(simple1 mesh_processing)

add_executable(simple2 examples/simple2/simple2.cpp)
target_link_libraries(simple2 mesh_processing)

