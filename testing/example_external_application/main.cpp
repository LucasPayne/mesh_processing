#include "mesh_processing/mesh_processing.h"
#include "mesh_processing/extensions/assimp_convert.h"
#include <stdio.h>


int main(void)
{
    SurfaceGeometry *geom = assimp_to_surface_geometry("Tangram-6.stl");
    std::cout << geom->num_vertices() << "\n";
    std::cout << geom->num_faces() << "\n";
    getchar();

    std::cout << geom->mesh.num_boundary_loops() << "\n";

    geom.mesh.lock();
    
    // auto mesh = Enmesh::grid_mesh(10, 10);
    // std::cout << mesh.num_vertices() << "\n";
}
