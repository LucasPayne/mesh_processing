#include "mesh_processing/mesh_processing.h"
#include <algorithm>//reverse

std::vector<Halfedge> SurfaceMesh::boundary_loops()
{
    return m_boundary_loops;
}

size_t SurfaceMesh::num_boundary_loops() const
{
    return m_boundary_loops.size();
}

bool SurfaceMesh::locked() const {
    return m_locked;
}

void SurfaceMesh::lock()
{
    if (m_locked) return;
    
    // "Unlocked":
    //     - Halfedges do not need to have twins.
    //     - A boundary halfedge is known by the fact that twin() is null.
    // "Locked":
    //     - Has a list of boundary halfedges, one for each boundary loop.
    //     - Boundary halfedges form a next() loop around the boundary "face", although the face() is null.
    //     - A boundary halfedge is known by the fact that face() is null.

    HalfedgeAttachment<char> visited(*this); //note: Something goes wrong with bool (maybe because std::vector<bool> is actually a different data structure).
    for (auto he : halfedges()) {
        visited[he] = false; //todo: It would be convenient to have default values for attachments.
    }

    std::vector<std::vector<Halfedge>> loops;
    
    // Look for a starting halfedges to form boundary loops.
    for (auto start : halfedges()) {
        if (start.twin().null() && !visited[start]) {
            // start is on an unvisited boundary loop.
            loops.push_back(std::vector<Halfedge>(0));
            auto &loop = loops[loops.size()-1];
            
            // Visit all the halfedges in the loop.
            auto he = start.next();
            while (true) {
                while (!he.twin().null()) {
                    he = he.twin().next();
                }
                loop.push_back(he);
                visited[he] = true;
                if (he == start) break;
                he = he.next();
            }
        }
    }
    
    m_boundary_loops.clear(); // m_boundary_loops will contain a starting halfedge for each boundary loop.
    for (auto &loop : loops) {
        std::vector<Halfedge> boundary_halfedges;
        for (size_t i = 0; i < loop.size(); i++) {
            auto u = loop[i].vertex();
            auto v = loop[i].tip();
            
            // Create a new boundary halfedge v->u, with a null face().
            assert(get_halfedge(v, u).null());
            auto he = Halfedge(*this, halfedge_pool.add());
            // Add this to the halfedge_map, which is used to quickly find halfedges between two vertices.
            halfedge_map[std::pair<ElementIndex,ElementIndex>(v.index(), u.index())] = he.index();
            // Set up incidence information for this boundary halfedge.
            he.set_face(Face(*this, InvalidElementIndex));
            he.set_vertex(v);
            he.set_twin(loop[i]);
            boundary_halfedges.push_back(he);
        }
        // Join these boundary halfedges into a loop around the non-existent "face".
        for (size_t i = 0; i < loop.size(); i++) {
            size_t prev_i = i>0 ? i-1 : loop.size()-1;
            boundary_halfedges[i].set_next(boundary_halfedges[prev_i]);
        }
        m_boundary_loops.push_back(boundary_halfedges[0]); // (The first boundary halfedge is the beginning of iterations around the boundary.)
    }

    // Test vertex manifoldness.
    //------------------------------------------------------------
    // A non-manifold vertex has disjoint triangle fans.
    // Consider a mesh of two triangles meeting at one vertex. The computed boundary loops will be disjoint,
    // as traversal from the perspective of triangle 1 was restricted to triangle 1. Therefore we have two boundary loops
    // with a common vertex. This is a sufficient condition for this vertex to be non-manifold.
    // Consider a mesh that looks like
    //      ---------
    //      | |~~~\ |
    //      | |    \|  <--- One connected piece, one boundary loop, but meets itself at a vertex.
    //      | |    /|
    //      | |___/ |
    //      ---------
    // This is another sufficient condition for that vertex to be non-manifold.
    // These two conditions are necessary and sufficient. (todo: Need to properly prove this).
    VertexAttachment<char> vertex_visited(*this);
    for (auto v : vertices()) {
        vertex_visited[v] = false;
    }
    for (auto start : m_boundary_loops) {
        auto he = start;
        do {
            if (vertex_visited[he.vertex()]) {
                std::cerr << "SurfaceMesh topology error: Non-manifold vertex.\n";
                exit(EXIT_FAILURE);
            }
            vertex_visited[he.vertex()] = true;
        } while ((he = he.next()) != start);
    }

    m_locked = true;
}


void SurfaceMesh::unlock()
{
    if (!m_locked) return;

    // Remove boundary loops.
    for (Halfedge start : boundary_loops()) {
        Halfedge he = start;
        std::vector<Halfedge> loop;
        do {
            loop.push_back(he);
        } while ((he = he.next()) != start);
        for (auto he : loop) halfedge_pool.remove(he.index());
    }


    m_locked = false;
}


