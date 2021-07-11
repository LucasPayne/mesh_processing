#include "mesh_processing/mesh_processing.h"
#include "mesh_processing/extensions/assimp_convert.h"
#include <stdio.h>


int main(void)
{
    // SurfaceGeometry *geom = assimp_to_surface_geometry("Tangram-6.stl");
    SurfaceGeometry *geom = assimp_to_surface_geometry("die.stl");
    geom->mesh.lock();

    auto tris = CompactTriangleMesh(*geom);
    std::cout << tris.num_vertices() << "\n";
    std::cout << tris.num_triangles() << "\n";

    // auto tet = TetgenMesh(*geom);
}
