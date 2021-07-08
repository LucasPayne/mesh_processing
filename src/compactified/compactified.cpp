#include "mesh_processing/mesh_processing.h"

// Construct a simple triangle list from a SurfaceGeometry (vertex positions attached to a SurfaceMesh).
CompactTriangleMesh::CompactTriangleMesh(SurfaceGeometry &geom)
{
    assert(geom.mesh.is_triangular());

    m_num_vertices = geom.mesh.num_vertices();
    m_num_triangles = geom.mesh.num_faces();
    m_position_data = Eigen::MatrixXf(m_num_vertices, 3);
    m_triangle_data = Eigen::MatrixXi(m_num_triangles, 3);

    int vertex_index = 0;
    VertexAttachment<int> contiguous_vertex_indices(geom.mesh);
    for (auto v : geom.mesh.vertices()) {
        m_position_data.row(vertex_index) = geom.position[v];
        contiguous_vertex_indices[v] = vertex_index;
        vertex_index += 1;
    }
    
    size_t face_index = 0;
    for (auto face : geom.mesh.faces()) {
        auto he = face.halfedge();
        int index_a = contiguous_vertex_indices[he.vertex()];
        he = he.next();
        int index_b = contiguous_vertex_indices[he.vertex()];
        he = he.next();
        int index_c = contiguous_vertex_indices[he.vertex()];
        m_triangle_data(face_index, 0) = index_a;
        m_triangle_data(face_index, 1) = index_b;
        m_triangle_data(face_index, 2) = index_c;
        face_index += 1;
    }
}

vec_t CompactTriangleMesh::position(size_t vertex_index) const
{
    return m_position_data.row(vertex_index);
}

int CompactTriangleMesh::triangle_vertex_index(size_t triangle_index, size_t vertex_number) const
{
    return m_triangle_data(triangle_index, vertex_number);
}
