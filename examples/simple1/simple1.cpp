#include <stdio.h>
#include "mesh_processing.h"

int main(void)
{
    // Create a quad.
    auto mesh = SurfaceMesh();
    auto v1 = mesh.add_vertex();
    auto v2 = mesh.add_vertex();
    auto v3 = mesh.add_vertex();
    auto v4 = mesh.add_vertex();
    mesh.add_triangle(v1, v3, v4);
    mesh.add_triangle(v1, v4, v2);

    printf("Done.\n");
}

