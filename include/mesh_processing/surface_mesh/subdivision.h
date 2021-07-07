#ifndef MESH_PROCESSING_SURFACE_MESH_SUBDIVISION_H
#define MESH_PROCESSING_SURFACE_MESH_SUBDIVISION_H

class TriangularSubdivision {
public:
    inline SurfaceMesh &original_mesh()
    {
        return *m_original_mesh;
    }
    inline SurfaceMesh &mesh()
    {
        return m_subdiv_mesh;
    }

    TriangularSubdivision(SurfaceMesh &_original_mesh);

private:
    SurfaceMesh *m_original_mesh;
    SurfaceMesh m_subdiv_mesh;

    EdgeAttachment<Vertex> edge_split_vertex;
    VertexAttachment<Vertex> vertex_to_vertex;

    friend class SurfaceMesh;
};


#endif // MESH_PROCESSING_SURFACE_MESH_SUBDIVISION_H
