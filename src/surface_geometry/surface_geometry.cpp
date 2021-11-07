#include "mesh_processing/mesh_processing.h"


float SurfaceGeometry::triangle_area(Face tri) const
{
    assert(tri.num_vertices() == 3);
    Vertex verts[3];
    int vertex_index = 0;
    auto he = tri.halfedge();
    do {
        he = he.next();
        verts[vertex_index] = he.vertex();
    } while (++vertex_index < 3);

    auto A = position[verts[0]];
    auto B = position[verts[1]];
    auto C = position[verts[2]];
    return 0.5f*(B-A).cross(C-A).norm();
}


vec_t SurfaceGeometry::barycenter(Face face) const
{
    vec_t avg = vec_t(0,0,0);

    auto start = face.halfedge();
    auto he = start;
    int num_vertices = 0;
    do {
        avg += position[he.vertex()];
        num_vertices += 1;
        he = he.next();
    } while (he != start);
    avg /= num_vertices;
    return avg;
}

vec_t SurfaceGeometry::triangle_normal(Face tri) const
{
    assert(tri.num_vertices() == 3);
    vec_t A = position[tri.halfedge().vertex()];
    vec_t B = position[tri.halfedge().next().vertex()];
    vec_t C = position[tri.halfedge().next().next().vertex()];
    vec_t n = (B-A).cross(C-A);
    return n / n.norm();
}

vec_t SurfaceGeometry::midpoint(Halfedge he) const
{
    return 0.5*position[he.vertex()] + 0.5*position[he.tip()];
}
vec_t SurfaceGeometry::midpoint(Edge edge) const
{
    return 0.5*position[edge.a().vertex()] + 0.5*position[edge.b().vertex()];
}


// SurfaceGeometry::SurfaceGeometry(CompactTriangleMesh &tris)
// {
//     for (int i = 0; i < tris.num_vertices(); i++) {
//         auto v = mesh.add_vertex();
//         position[v] = tris.position[i];
//     }
//     for (int i = 0; i < tris.num_faces(); i++) {
//         auto a = Vertex(tris.triangle_vertex_index(i, 0), mesh);
//         auto b = Vertex(tris.triangle_vertex_index(i, 0), mesh);
//         auto c = Vertex(tris.triangle_vertex_index(i, 0), mesh); // Important: Assumption of contiguous indices.
//         mesh.add_triangle(a, b, c);
//     }
// }
