#include "mesh_processing/mesh_processing.h"


SurfaceGeometry::SurfaceGeometry(CompactTriangleMesh &tris) :
    mesh(*new SurfaceMesh())
{
    for (int i = 0; i < tris.num_vertices(); i++) {
        auto v = mesh.add_vertex();
        position[v] = tris.position[i];
    }
    for (int i = 0; i < tris.num_faces(); i++) {
        auto a = Vertex(tris.triangle_vertex_index(i, 0), mesh);
        auto b = Vertex(tris.triangle_vertex_index(i, 0), mesh);
        auto c = Vertex(tris.triangle_vertex_index(i, 0), mesh); // Important: Assumption of contiguous indices.
        mesh.add_triangle(a, b, c);
    }
}
