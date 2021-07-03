#include <stdio.h>
#include <fstream>
#include "mesh_processing.h"

#define PATH "../examples/simple2/"


int main(void)
{
    auto grid = Enmesh::grid_mesh(10, 10);

    grid.lock_topology();
    for (auto start : grid.boundary_loops()) {
        printf("Loop\n");
        auto he = start;
        do {
            printf("%d\n", he.vertex().index());
        } while ((he = he.next()) != start);
    }
}

