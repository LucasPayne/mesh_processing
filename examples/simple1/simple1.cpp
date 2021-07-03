#include <stdio.h>
#include <fstream>
#include "mesh_processing.h"

#define PATH "../examples/simple1/"


int main(void)
{
    auto quad = SurfaceMesh();
    auto v1 = quad.add_vertex();
    auto v2 = quad.add_vertex();
    auto v3 = quad.add_vertex();
    auto v4 = quad.add_vertex();
    printf("%d %d %d %d\n", v1.index(),v2.index(),v3.index(),v4.index());
    getchar();

    quad.add_triangle(v1, v2, v3);
    quad.add_triangle(v1, v3, v4);

    quad.lock_topology();

    // auto grid = Enmesh::grid_mesh(10, 10);

    // auto greenland = Enmesh::load_geometry(PATH "greenland.mesh");
    // std::cout << greenland.num_vertices() << "\n";
    // std::cout << greenland.num_faces() << "\n";

    // Enmesh::save_geometry(greenland, PATH "greenland_copy.mesh");

    printf("Done.\n");
}

