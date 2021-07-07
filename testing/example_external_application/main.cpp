#include "mesh_processing/mesh_processing.h"
#include "mesh_processing/extensions/assimp_convert.h"
#include <stdio.h>


int main(void)
{
    SurfaceGeometry *geom = assimp_to_surface_geometry("Tangram-6.stl");
    geom->mesh.lock();

    std::cout << geom->mesh.num_vertices() << "\n";
    std::cout << geom->mesh.num_edges() << "\n";

    auto mesh_subdiv = TriangularSubdivision(geom->mesh);

    std::cout << mesh_subdiv.mesh().num_vertices() << "\n";
}
