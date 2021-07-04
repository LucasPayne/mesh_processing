#include <stdio.h>
#include <fstream>
#include "mesh_processing/mesh_processing.h"

#define PATH "../examples/simple1/"


int main(void)
{
    auto quad = SurfaceMesh();
    auto v1 = quad.add_vertex();
    auto v2 = quad.add_vertex();
    auto v3 = quad.add_vertex();
    auto v4 = quad.add_vertex();
    // printf("%d %d %d %d\n", v1.index(),v2.index(),v3.index(),v4.index());
    // getchar();

    // auto t1 = quad.add_triangle(v1, v2, v3);
    // auto t2 = quad.add_triangle(v1, v3, v4);
    // quad.remove_face(t2);
    // quad.remove_vertex(v4);
    
    auto v_center = quad.add_vertex();
    auto t1 = quad.add_triangle(v1, v2, v_center);
    auto t2 = quad.add_triangle(v4, v3, v_center);

    quad.lock();
    for (auto start : quad.boundary_loops()) {
        printf("Loop\n");
        auto he = start;
        do {
            printf("%d\n", he.vertex().index());
        } while ((he = he.next()) != start);
    }

    // auto grid = Enmesh::grid_mesh(10, 10);

    // auto greenland = Enmesh::load_geometry(PATH "greenland.mesh");
    // std::cout << greenland.num_vertices() << "\n";
    // std::cout << greenland.num_faces() << "\n";
    // greenland.mesh.lock();
    // Enmesh::save_geometry(greenland, PATH "greenland_copy.mesh");

    printf("Done.\n");
}

