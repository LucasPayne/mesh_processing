#ifndef MESH_PROCESSING_ASSIMP_CONVERT_H
#define MESH_PROCESSING_ASSIMP_CONVERT_H

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <stdint.h>
#include <algorithm>
#include <limits>

SurfaceGeometry assimp_to_surface_geometry(const std::string &filename);

#endif // MESH_PROCESSING_ASSIMP_CONVERT_H
