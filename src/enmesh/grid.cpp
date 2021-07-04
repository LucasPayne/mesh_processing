#include "mesh_processing/mesh_processing.h"
#include <assert.h>
namespace Enmesh {

/*
 * x_nodes-by-y_nodes grid, with two triangles per quad.
 * x_nodes is the number of horizontal nodes. There will be x_nodes-1 horizontal intervals.
 */
SurfaceMesh grid_mesh(int x_nodes, int y_nodes)
{
    assert(x_nodes > 1 && y_nodes > 1);
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

SurfaceGeometry grid_geom(int x_nodes, int y_nodes, float bl_x, float bl_y, float tr_x, float tr_y)
{
    assert(x_nodes > 1 && y_nodes > 1);
    // auto geom = SurfaceGeometry(grid_mesh(x_nodes, y_nodes)); ---todo: Geometry from mesh
    auto geom = SurfaceGeometry();
    std::vector<Vertex> grid_vertices(x_nodes * y_nodes);

    float inv_x = 1.f / (x_nodes - 1);
    float inv_y = 1.f / (y_nodes - 1);
    for (int i = 0; i < x_nodes; i++) {
        for (int j = 0; j < y_nodes; j++) {
            float tx = i * inv_x;
            float ty = j * inv_y;
            grid_vertices[x_nodes*j + i] = geom.add_vertex((1-tx)*bl_x + tx*tr_x, (1-ty)*bl_y + ty*tr_y, 0);
        }
    }
    for (int i = 0; i < x_nodes-1; i++) {
        for (int j = 0; j < y_nodes-1; j++) {
            auto &bl = grid_vertices[x_nodes*j + i];
            auto &br = grid_vertices[x_nodes*j + i+1];
            auto &tr = grid_vertices[x_nodes*(j+1) + i+1];
            auto &tl = grid_vertices[x_nodes*(j+1) + i];
            geom.mesh.add_triangle(bl, br, tr);
            geom.mesh.add_triangle(bl, tr, tl);
        }
    }
    return geom;
}

}; // namespace Enmesh
