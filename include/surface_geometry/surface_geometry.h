#ifndef SURFACE_GEOMTRY_H
#define SURFACE_GEOMTRY_H


using vec_t = Eigen::Vector3f;

class SurfaceGeometry {
public:
    SurfaceMesh mesh;

    Vertex add_vertex(vec_t position);
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

    VertexAttachment<vec_t> vertex_positions;
    VertexAttachment<vec_t> vertex_normals;
    FaceAttachment<vec_t> face_normals;
    
    // Input/output.
    void write_OFF(std::ostream &out);
    void read_OFF(std::istream &in);
};

#endif // SURFACE_GEOMTRY_H
