#include "mesh_processing/mesh_processing.h"
namespace Subdivision {

Triangular::Triangular(SurfaceMesh &_original_mesh) :
    m_original_mesh{&_original_mesh},
    m_subdiv_mesh(),
    m_edge_split_vertex(_original_mesh),
    m_vertex_to_vertex(_original_mesh)
{
    assert(original_mesh().locked());
    assert(original_mesh().is_triangular());

    for (auto v : original_mesh().vertices()) {
        m_vertex_to_vertex[v] = mesh().add_vertex();
    }
    for (auto edge : original_mesh().edges()) {
        m_edge_split_vertex[edge] = mesh().add_vertex();
    }

    for (auto face : original_mesh().faces()) {
        // Add the three outer triangles.
        auto start = face.halfedge();
        auto he = start;
        Vertex center_triangle_vertices[3];
        int i = 0;
        do {
            auto vertex1 = m_edge_split_vertex[he.edge()];
            auto vertex2 = m_vertex_to_vertex[he.next().vertex()];
            auto vertex3 = m_edge_split_vertex[he.next().edge()];
            mesh().add_triangle(vertex1, vertex2, vertex3);
            center_triangle_vertices[i] = vertex1;
            i ++;
        } while ((he = he.next()) != start);
        // Add center triangle.
        mesh().add_face(center_triangle_vertices, 3);
    }
    mesh().lock();
}


SurfaceGeometry *loop(Triangular &subdiv, SurfaceGeometry &geom)
{
    // Loop subdivision.
    // Input: A Subdivision::Triangular instance, topologically subdividing a mesh.
    //        A SurfaceGeometry giving vertex positions for the original mesh (the one subdivided).
    // Returns: A SurfaceGeometry for the subdivided mesh, with vertex positions given by the Loop subdivision rules.
    assert(&subdiv.original_mesh() == &geom.mesh);
    SurfaceMesh &original_mesh = subdiv.original_mesh();

    auto subdiv_geom = new SurfaceGeometry(subdiv.mesh());
    // Compute positions of vertex points.
    for (auto v : original_mesh.vertices()) {
        size_t n = v.num_adjacent_vertices();
        float _c = 3+ 2*cos(2*M_PI/n);
        float beta = (5.f/8.f - (_c*_c)/64.f)/n;
        float original_weight = 1-n*beta;
        float neighbour_weight = beta;
        vec_t pos = original_weight * geom.position[v];
        auto start = v.halfedge();
        auto he = start;
        do {
            pos += neighbour_weight * geom.position[he.twin().vertex()];
        } while ((he = he.twin().next()) != start);
        subdiv_geom->position[subdiv.corresponding_vertex(v)] = pos;
    }
    // Compute positions of new edge points.
    for (auto edge : original_mesh.edges()) {
        auto end_a = geom.position[edge.a().vertex()];
        auto end_b = geom.position[edge.b().vertex()];
        auto wing_a = geom.position[edge.a().next().next().vertex()];
        auto wing_b = geom.position[edge.b().next().next().vertex()];
        vec_t pos = (1.0/8.0)*wing_a + (1.0/8.0)*wing_b + (3.0/8.0)*end_a + (3.0/8.0)*end_b;
        subdiv_geom->position[subdiv.edge_split_vertex(edge)] = pos;
    }
    return subdiv_geom;
}


}; // namespace Subdivision
