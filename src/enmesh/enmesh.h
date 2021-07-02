#ifndef MESH_PROCESSING_ENMESH_H
#define MESH_PROCESSING_ENMESH_H
namespace Enmesh {

SurfaceMesh grid_mesh(int x_nodes, int y_nodes);

SurfaceGeometry load_geometry(const std::string &filename);


}; // namespace Enmesh
#endif // MESH_PROCESSING_ENMESH_H
