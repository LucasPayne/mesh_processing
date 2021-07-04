cmake_minimum_required(VERSION 3.1)
project(example_external_application)

set(MESH_PROCESSING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
set(MESH_PROCESSING_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../build/libmesh_processing.a)

set(EIGEN3_INCLUDE_DIR "usr/include/eigen3")
find_package(Eigen3 REQUIRED)


add_library(mesh_processing SHARED IMPORTED GLOBAL)
set_target_properties(mesh_processing PROPERTIES IMPORTED_LOCATION ${MESH_PROCESSING_LIBRARY_PATH})

include_directories(. ${MESH_PROCESSING_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR})
add_executable(example_external_application
    main.cpp
)
target_link_libraries(example_external_application mesh_processing)
