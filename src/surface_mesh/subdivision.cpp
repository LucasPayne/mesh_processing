#include "mesh_processing/mesh_processing.h"


TriangularSubdivision::TriangularSubdivision(SurfaceMesh &_original_mesh) :
    m_original_mesh{&_original_mesh},
    edge_split_vertex(_original_mesh),
    vertex_to_vertex(_original_mesh)
{
    assert(original_mesh().locked());
    assert(original_mesh().is_triangular());

    for (auto v : original_mesh().vertices()) {
        vertex_to_vertex[v] = mesh().add_vertex();
    }
    for (auto edge : original_mesh().edges()) {
        edge_split_vertex[edge] = mesh().add_vertex();
    }

    for (auto face : original_mesh().faces()) {
        // Add the three outer triangles.
        auto start = face.halfedge();
        auto he = start;
        Vertex center_triangle_vertices[3];
        int i = 0;
        do {
            auto vertex1 = edge_split_vertex[he.edge()];
            auto vertex2 = vertex_to_vertex[he.next().vertex()];
            auto vertex3 = edge_split_vertex[he.next().edge()];
            mesh().add_triangle(vertex1, vertex2, vertex3);
            center_triangle_vertices[i] = vertex1;
            i ++;
        } while ((he = he.next()) != start);
        // Add center triangle.
        mesh().add_face(center_triangle_vertices, 3);
    }
}
