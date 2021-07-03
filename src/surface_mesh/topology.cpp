#include "mesh_processing.h"
#include "logging.h"
#include <algorithm>//reverse

std::vector<Halfedge> SurfaceMesh::boundary_loops()
{
    return m_boundary_loops;
}

size_t SurfaceMesh::num_boundary_loops() const
{
    return m_boundary_loops.size();
}

void SurfaceMesh::lock_topology()
{
    // "Unlocked topology":
    //     - Halfedges do not need to have twins.
    //     - A boundary halfedge is known by the fact that twin() is null.
    // "Locked topology":
    //     - Has a list of boundary halfedges, one for each boundary loop.
    //     - Boundary halfedges form a next() loop around the boundary "face", although the face() is null.
    //     - A boundary halfedge is known by the fact that face() is null.

    HalfedgeAttachment<int> visited(*this); //note: Something goes wrong with bool (maybe because std::vector<bool> is actually a different data structure).
    for (auto he : halfedges()) {
        visited[he] = false; //todo: It would be convenient to have default values for attachments.
    }

    std::vector<std::vector<Halfedge>> loops;
    

    // Look for a starting halfedge to form a boundary loop.
    for (auto start : halfedges()) {
        if (start.twin().null() && !visited[start]) {
            // start is on an unvisited boundary loop.
            loops.push_back(std::vector<Halfedge>(0));
            auto &loop = loops[loops.size()-1];

            printf("Found loop\n");
	    printf("start: %d %d\n", start.vertex().index(), start.tip().index());
            getchar();
            
            auto he = start.next();
            while (true) {
                while (!he.twin().null()) {
                    he = he.twin().next();
                }
                loop.push_back(he);
                visited[he] = true;
                printf("%d %d\n", he.vertex().index(), he.tip().index());
                getchar();
                if (he == start) break;
                he = he.next();
            }
        }
    }

    std::cout << loops.size() << "\n";
    getchar();
}


void SurfaceMesh::unlock_topology()
{

}
