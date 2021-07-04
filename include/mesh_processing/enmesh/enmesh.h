#ifndef MESH_PROCESSING_ENMESH_H
#define MESH_PROCESSING_ENMESH_H
namespace Enmesh {

SurfaceMesh grid_mesh(int x_nodes, int y_nodes);
SurfaceGeometry grid_geom(int x_nodes, int y_nodes, float bl_x, float bl_y, float tr_x, float tr_y);

void save_geometry(SurfaceGeometry &geom, const std::string &filename);
SurfaceGeometry load_geometry(const std::string &filename);


}; // namespace Enmesh
#endif // MESH_PROCESSING_ENMESH_H
