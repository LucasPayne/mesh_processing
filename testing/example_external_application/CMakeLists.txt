cmake_minimum_required(VERSION 3.1)
project(example_external_application)

set(ASSIMP_SOURCE_DIR /home/lucas/computer_graphics/assimp)
set(ASSIMP_INCLUDE_DIR ${ASSIMP_SOURCE_DIR}/include)
set(ASSIMP_LIBRARY_PATH ${ASSIMP_SOURCE_DIR}/bin/libassimp.so)
set(MESH_PROCESSING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
set(MESH_PROCESSING_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../build/libmesh_processing.a)

set(EIGEN3_INCLUDE_DIR "usr/include/eigen3")
find_package(Eigen3 REQUIRED)


add_library(mesh_processing SHARED IMPORTED GLOBAL)
set_target_properties(mesh_processing PROPERTIES IMPORTED_LOCATION ${MESH_PROCESSING_LIBRARY_PATH})

add_library(assimp SHARED IMPORTED GLOBAL)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${ASSIMP_LIBRARY_PATH})
message(STATUS "assimp path: ${ASSIMP_LIBRARY_PATH}")

include_directories(. ${ASSIMP_INCLUDE_DIR} ${MESH_PROCESSING_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR})
add_executable(example_external_application
    main.cpp
)
target_link_libraries(example_external_application mesh_processing assimp)
