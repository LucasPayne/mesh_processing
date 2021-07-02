#include "mesh_processing/surface_geometry/surface_geometry.h"
#include "mesh_processing/logging.h"
#include <iomanip>


Vertex SurfaceGeometry::add_vertex(vec3 position)
{
    auto vertex = mesh.add_vertex();
    vertex_positions[vertex] = position;
    return vertex;
}

Vertex SurfaceGeometry::add_vertex(float x, float y, float z)
{
    return add_vertex(vec3(x, y, z));
}

Face SurfaceGeometry::add_triangle(Vertex v1, Vertex v2, Vertex v3)
{
    vec3 a = vertex_positions[v1];
    vec3 b = vertex_positions[v2];
    vec3 c = vertex_positions[v3];
    vec3 n = vec3::cross(b - a, c - a).normalized();
    auto face = mesh.add_triangle(v1, v2, v3);
    if (face.null()) return face;
    face_normals[face] = n;
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
        vec3 position = vertex_positions[vertex];
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



void SurfaceGeometry::add_model(MLModel &model)
{
    std::vector<Vertex> new_vertices(model.num_vertices);
    for (unsigned int i = 0; i < model.num_vertices; i++) {
        new_vertices[i] = add_vertex(model.positions[i]);
    }

    if (model.has_triangles) {
        for (auto triangle : model.triangles) {
            add_triangle(new_vertices[triangle.a],
                         new_vertices[triangle.b],
                         new_vertices[triangle.c]);
        }
    }
}


void SurfaceGeometry::read_OFF(std::istream &in)
{
    auto model = MLModel::load(MODEL_FILE_FORMAT_OFF, in);
    add_model(model);
}


MLModel SurfaceGeometry::to_model()
{
    MLModel model;
    model.name = "N/A";
    model.num_vertices = 3 * num_faces();
    model.positions = std::vector<vec3>(model.num_vertices);
    model.normals = std::vector<vec3>(model.num_vertices);
    model.has_normals = true;
    model.num_triangles = num_faces();
    model.triangles = std::vector<MLModelTriangle>(model.num_triangles);
    model.has_triangles = true;

    int i = 0;
    for (auto face : mesh.faces()) {
        if (face.num_vertices() != 3) {
            log_error("[surface_geometry] Cannot convert non-triangular mesh to an MLModel.");
            exit(EXIT_FAILURE);
        }
        auto start = face.halfedge();
        auto he = start;
        int j = 0;
        vec3 normal = face_normals[face];
        do {
            model.positions[3*i + j] = vertex_positions[he.vertex()];
            model.normals[3*i + j] = normal;
            he = he.next();
            j += 1;
        } while (start != he);
        model.triangles[i] = MLModelTriangle(3*i+0, 3*i+1, 3*i+2);
        i += 1;
    }
    return model;
}
