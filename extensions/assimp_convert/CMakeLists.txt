cmake_minimum_required(VERSION 3.1)

include_directories(../../include assimp/include)

add_library(assimp_convert
    assimp_convert.cpp
)
