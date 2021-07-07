#include "mesh_processing/mesh_processing.h"


//SurfaceMeshTriangularSubdivision SurfaceMesh::subdivide_triangles()
void SurfaceMesh::subdivide_triangles()
{
    assert(locked());
    assert(is_triangular());

    auto subdiv = SurfaceMeshTriangularSubdivision(*this);

    for (auto edge : edges()) {
        
    }

    // return subdiv;
}

