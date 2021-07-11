#ifndef SURFACE_GEOMETRY_H
#define SURFACE_GEOMETRY_H

class CompactTriangleMesh;
using vec_t = Eigen::Vector3f;

class SurfaceGeometry {
public:
    SurfaceMesh &mesh;
    VertexAttachment<vec_t> position;

    SurfaceGeometry(SurfaceMesh &_mesh) :
        mesh{_mesh},
        position(mesh)
    {}

    // Convert from a simple list of vertex positions and triangle indices.
    // This creates the underlying mesh then attaches geometry positions.
    // SurfaceGeometry(CompactTriangleMesh &tris);
};

#endif // SURFACE_GEOMETRY_H
