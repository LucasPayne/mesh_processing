#ifndef SURFACE_GEOMETRY_H
#define SURFACE_GEOMETRY_H


using vec_t = Eigen::Vector3f;

class SurfaceGeometry {
public:
    SurfaceMesh &mesh;
    VertexAttachment<vec_t> position;

    SurfaceGeometry(SurfaceMesh &_mesh) :
        mesh{_mesh},
        position(mesh)
    {}
};

#endif // SURFACE_GEOMETRY_H
