#include "mesh_processing/mesh_processing.h"
namespace Enmesh {

/*
 * x_nodes-by-y_nodes grid, with two triangles per quad.
 * x_nodes is the number of horizontal nodes. There will be x_nodes-1 horizontal intervals.
 */
SurfaceMesh grid_mesh(int x_nodes, int y_nodes)
{
    auto mesh = SurfaceMesh();
    std::vector<Vertex> grid_vertices(x_nodes * y_nodes);
    for (int i = 0; i < x_nodes; i++) {
        for (int j = 0; j < y_nodes; j++) {
            grid_vertices[x_nodes*j + i] = mesh.add_vertex();
        }
    }
    for (int i = 0; i < x_nodes-1; i++) {
        for (int j = 0; j < y_nodes-1; j++) {
            auto &bl = grid_vertices[x_nodes*j + i];
            auto &br = grid_vertices[x_nodes*j + i+1];
            auto &tr = grid_vertices[x_nodes*(j+1) + i+1];
            auto &tl = grid_vertices[x_nodes*(j+1) + i];
            mesh.add_triangle(bl, br, tr);
            mesh.add_triangle(bl, tr, tl);
        }
    }
    return mesh;
}

}; // namespace Enmesh
