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
    quad.add_triangle(v1, v3, v4);
    quad.add_triangle(v1, v4, v2);

    auto grid = Enmesh::grid_mesh(10, 10);


    auto greenland = Enmesh::load_geometry(PATH "greenland.mesh");

    printf("Done.\n");
}

