#include <stdio.h>
#include <iostream>
#include <fstream>
#include "mesh_processing/mesh_processing.h"

#define PATH "../examples/simple2/"


int main(void)
{
    auto grid = Enmesh::grid_mesh(10, 10);
    // auto grid2 = Enmesh::grid_mesh(5, 5);
    // grid.add(grid2);
    grid.lock();
    // for (auto face : grid.connected_components()) {
    //     grid.remove_connected_component(face);
    //     break;
    // }
    // std::cout << grid.num_connected_components() << "\n";

    std::cout << grid.num_halfedges() << "\n";
    std::cout << grid.num_edges() << "\n";

    auto subdiv = TriangularSubdivision(grid);


    // for (auto start : grid.boundary_loops()) {
    //     printf("Loop\n");
    //     auto he = start;
    //     do {
    //         printf("%d\n", he.vertex().index());
    //     } while ((he = he.next()) != start);
    // }

    // std::cout << grid.num_connected_components() << "\n";
}

