#ifndef MESH_PROCESSING_SURFACE_MESH_SUBDIVISION_H
#define MESH_PROCESSING_SURFACE_MESH_SUBDIVISION_H
namespace Subdivision {

class Triangular {
public:
    inline SurfaceMesh &original_mesh()
    {
        return *m_original_mesh;
    }
    inline SurfaceMesh &mesh()
    {
        return m_subdiv_mesh;
    }

    Triangular(SurfaceMesh &_original_mesh);

    inline Vertex corresponding_vertex(Vertex original_vertex)
    {
        return m_vertex_to_vertex[original_vertex];
    }
    inline Vertex edge_split_vertex(Edge original_edge)
    {
        return m_edge_split_vertex[original_edge];
    }

private:
    SurfaceMesh *m_original_mesh;
    SurfaceMesh m_subdiv_mesh;

    EdgeAttachment<Vertex> m_edge_split_vertex;
    VertexAttachment<Vertex> m_vertex_to_vertex;

    friend class SurfaceMesh;
};

SurfaceGeometry *loop(Triangular &subdiv, SurfaceGeometry &geom);
SurfaceGeometry *barycentric(Triangular &subdiv, SurfaceGeometry &geom);

}; // namespace Subdivision
#endif // MESH_PROCESSING_SURFACE_MESH_SUBDIVISION_H
