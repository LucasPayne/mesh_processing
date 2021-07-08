#ifndef MESH_PROCESSING_COMPACITIFIED
#define MESH_PROCESSING_COMPACITIFIED
#include <Eigen/Dense>
// Note on terminology
// -------------------
// Compactified data structures usually contain position/embedding information. The term "mesh" used here does not imply purely topological.


class CompactTriangleMesh {
public:
    CompactTriangleMesh(SurfaceGeometry &geom);

    vec_t position(size_t vertex_index) const;
    int triangle_vertex_index(size_t face_index, size_t vertex_number) const;

    inline size_t num_vertices() const { return m_num_vertices; }
    inline size_t num_triangles() const { return m_num_triangles; }

private:
    Eigen::MatrixXf m_position_data;
    Eigen::MatrixXi m_triangle_data;
    size_t m_num_vertices;
    size_t m_num_triangles;
};


#endif // MESH_PROCESSING_COMPACITIFIED
