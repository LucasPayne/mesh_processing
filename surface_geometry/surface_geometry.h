#ifndef SURFACE_GEOMTRY_H
#define SURFACE_GEOMTRY_H
#include "spatial_algebra/spatial_algebra.h"
#include "mesh_processing/surface_mesh/surface_mesh.h"



class SurfaceGeometry {
public:
    SurfaceMesh mesh;

    Vertex add_vertex(vec3 position);
    Vertex add_vertex(float x, float y, float z);
    Face add_triangle(Vertex v1, Vertex v2, Vertex v3);

    SurfaceGeometry() :
        mesh(),
        vertex_positions(mesh),
        vertex_normals(mesh),
        face_normals(mesh)
    {}

    SurfaceMesh::ElementContainer<Vertex> vertices() { return mesh.vertices(); }
    SurfaceMesh::ElementContainer<Edge> edges() { return mesh.edges(); }
    SurfaceMesh::ElementContainer<Face> faces() { return mesh.faces(); }

    size_t num_vertices() const { return mesh.num_vertices(); }
    size_t num_edges() const { return mesh.num_edges(); }
    size_t num_faces() const { return mesh.num_faces(); }

    void printout();

    VertexAttachment<vec3> vertex_positions;
    VertexAttachment<vec3> vertex_normals;
    FaceAttachment<vec3> face_normals;


    // model_loader library related methods.

    // Add the geometry of the given model to the surface.
    void add_model(MLModel &model);

    // Input/output.
    void write_OFF(std::ostream &out);
    void read_OFF(std::istream &in);

    // Convert the SurfaceGeometry into a triangle-soup model with per-facet normals.
    MLModel to_model();
};


#endif // SURFACE_GEOMTRY_H
