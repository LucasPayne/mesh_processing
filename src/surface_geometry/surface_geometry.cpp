#include "mesh_processing.h"
#include <assert.h>
#include <iomanip>


Vertex SurfaceGeometry::add_vertex(vec_t position)
{
    auto vertex = mesh.add_vertex();
    vertex_positions[vertex] = position;
    return vertex;
}

Vertex SurfaceGeometry::add_vertex(float x, float y, float z)
{
    return add_vertex(vec_t(x, y, z));
}

Face SurfaceGeometry::add_triangle(Vertex v1, Vertex v2, Vertex v3)
{
    vec_t a = vertex_positions[v1];
    vec_t b = vertex_positions[v2];
    vec_t c = vertex_positions[v3];
    // vec_t n = (b - a).cross(c - a).normalized();
    auto face = mesh.add_triangle(v1, v2, v3);
    if (face.null()) return face;
    // face_normals[face] = n;
    return face;
}


void SurfaceGeometry::printout()
{
    mesh.printout();
    printf("SurfaceGeometry attachments\n");
    printf("------------------------------------------------------------\n");
    for (auto vertex : mesh.vertices()) {
        std::cout << vertex_positions[vertex] << "\n";
    }
    printf("------------------------------------------------------------\n");
    getchar();
}

void SurfaceGeometry::write_OFF(std::ostream &out)
{
    // Vertex indices in the ElementPool sense are not necessarily contiguous.
    // So, a temporary attachment is used to give contiguous indices to the vertices.
    auto vertex_indices = VertexAttachment<uint32_t>(mesh);

    out << "OFF\n";
    out << mesh.num_vertices() << " " << mesh.num_faces() << " 0\n";
    out << std::fixed << std::setprecision(6);
    int index = 0;
    for (auto vertex : mesh.vertices()) {
        vertex_indices[vertex] = index; // Give the vertices contiguous indices.
        vec_t position = vertex_positions[vertex];
        out << position.x() << " " << position.y() << " " << position.z() << "\n";
        index ++;
    }
    for (auto face : mesh.faces()) {
        out << face.num_vertices();
        auto start = face.halfedge();
        auto he = start;
        do {
            uint32_t vertex_index = vertex_indices[he.vertex()];
            out << " " << vertex_index;
            he = he.next();
        } while (start != he);
        out << "\n";
    }
}

void SurfaceGeometry::read_OFF(std::istream &in)
{
    std::cout << "Not implemented read_OFF!\n";
    assert(0);
}
