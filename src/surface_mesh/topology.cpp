#include "mesh_processing/mesh_processing.h"
#include <algorithm>


std::vector<Halfedge> SurfaceMesh::boundary_loops()
{
    assert(locked());
    return m_boundary_loops;
}

size_t SurfaceMesh::num_boundary_loops() const
{
    assert(locked());
    return m_boundary_loops.size();
}
bool SurfaceMesh::closed() const
{
    assert(locked());
    return m_boundary_loops.size() == 0;
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
    //     - Vertex->halfedge incidences are not valid.
    //     - An undirected edge make no sense, and Edge handles can't be used.
    // "Locked":
    //     - Has a list of boundary halfedges, one for each boundary loop.
    //     - Boundary halfedges form a next() loop around the boundary "face", although the face() is null.
    //     - A boundary halfedge is known by the fact that face() is null.
    //     - Vertex->halfedge incidences have been set up for mesh traversal.
    //     - Edge handles can be used.

    HalfedgeAttachment<char> visited(*this); //note: Something goes wrong with bool (maybe because std::vector<bool> is actually a different data structure).
    for (auto he : halfedges()) {
        visited[he] = false; //todo: It would be convenient to have default values for attachments.
    }

    std::vector<std::vector<Halfedge>> loops(0);
    
    printf("lock(): Computing boundary loops.\n");
    // Look for starting halfedges to form boundary loops.
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
            loop[i].set_twin(he);
            boundary_halfedges.push_back(he);
        }
        // Join these boundary halfedges into a loop around the non-existent "face".
        for (size_t i = 0; i < loop.size(); i++) {
            size_t prev_i = i>0 ? i-1 : loop.size()-1;
            boundary_halfedges[i].set_next(boundary_halfedges[prev_i]);
        }
        m_boundary_loops.push_back(boundary_halfedges[0]); // (The first boundary halfedge is the beginning of iterations around the boundary.)
    }

    printf("lock(): Testing vertex manifoldness.\n");
    // Test vertex manifoldness.
    //------------------------------------------------------------
    // A non-manifold vertex has disjoint triangle fans.
    // Consider a mesh of two triangles meeting at one vertex. The computed boundary loops will be disjoint,
    // as traversal from the perspective of triangle 1 was restricted to triangle 1.
    //      |\    /|
    //      | \  / |
    //      |  \/  |
    //      |  /\  |
    //      | /  \ |
    //      |/    \|
    // Therefore we have two boundary loops with a common vertex.
    // This is a sufficient condition for this vertex to be non-manifold.
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

    //------------------------------------------------------------
    // By now, the mesh has been topologically verified.
    //------------------------------------------------------------

    printf("lock(): Adding vertex->halfedge incidences.\n");
    // Add vertex->halfedge incidences.
    //------------------------------------------------------------
    for (auto v : vertices()) {
        vertex_visited[v] = false; // re-use this attachment.
    }
    // For each face, for all vertices on this face which have not had their halfedge set,
    // set it to the relevant halfedge on this face.
    // (This is a somewhat arbitrary choice. Each vertex needs just one of its outgoing halfedges.)
    for (auto face : faces()) {
        auto start = face.halfedge();
        auto he = start;
        do {
            if (!vertex_visited[he.vertex()]) {
                he.vertex().set_halfedge(he);
                vertex_visited[he.vertex()] = true;
            }
        } while ((he = he.next()) != start);
    }

    printf("lock(): Adding edge data.\n");
    // Add edge data. An "edge" only makes sense when the mesh is locked.
    //------------------------------------------------------------
    for (auto he : halfedges()) {
        visited[he] = false; // Re-use this attachment.
    }
    for (auto he : halfedges()) {
        if (visited[he] || visited[he.twin()]) continue;
        // Create a new edge.
        auto edge = Edge(*this, edge_pool.add());
        // Set up halfedges<->edge incidence data.
        edge.set_halfedge_a(he);
        edge.set_halfedge_b(he.twin());
        he.set_edge(edge);
        he.twin().set_edge(edge);
        visited[he] = true;
        visited[he.twin()] = true;
    }

    printf("lock(): Computing connected components.\n");
    // // Compute connected components
    // //------------------------------------------------------------
    // m_connected_components.clear();
    // // For each face, store a "visited" flag (which starts at false).
    // // Repeat this process until connected components are found:
    // //     Find the next face which is not visited.
    // //     This corresponds to a connected component.
    // //     Run search(face), which:
    // //         Marks face as visited.
    // //         Recurs: Finds adjacent non-visited faces, and runs search(face) on them.
    // FaceAttachment<char> face_visited(*this);
    // for (auto face : faces()) face_visited[face] = false;
    // std::function<void(Face)> search = [&](Face face) {
    //     face_visited[face] = true;
    //     auto start = face.halfedge();
    //     auto he = start;
    //     do {
    //         assert(!he.twin().null());
    //         if (!he.twin().face().null() && !face_visited[he.twin().face()]) {
    //             search(he.twin().face());
    //         }
    //     } while ((he = he.next()) != start);
    // };
    // for (auto face : faces()) {
    //     if (!face_visited[face]) {
    //         m_connected_components.push_back(face);
    //         search(face);
    //     }
    // };
    
    // Cache vertex boundary-ness, and count the number of interior vertices.
    for (auto v : vertices()) {
        vertex_on_boundary[v] = 0;
    }
    for (auto start : m_boundary_loops) {
        auto he = start;
        do {
            vertex_on_boundary[he.vertex()] = 1;
            he = he.next();
        } while (he != start);
    }

    // For each vertex on the boundary, make sure that the outgoing halfedge
    // has a nonnull face, and that this is the first face, such that all faces can be traversed.
    for (auto start : m_boundary_loops) {
        auto he = start;
        do {
            auto v = he.vertex();
            assert(!he.twin().next().null());
            v.set_halfedge(he.twin().next());
            he = he.next();
        } while (he != start);
    }
    m_locked = true;
    
    // Post-lock changes. NOTE: Be careful! Try to minimize the number of post-lock changes,
    // and make sure they don't assume that the whole mesh is "fully locked" (as the data below still has to be initialized).
    // Count the number of interior vertices.
    m_num_interior_vertices = 0;
    for (auto v : vertices()) {
        if (!v.on_boundary()) m_num_interior_vertices += 1;
    }
    // Count the number of interior edges.
    m_num_interior_edges = 0;
    for (auto edge : edges()) {
        if (!edge.on_boundary()) m_num_interior_edges += 1;
    }

    printf("lock(): Complete.\n");
}


void SurfaceMesh::unlock()
{
    if (!m_locked) return;

    // Remove boundary loops.
    for (Halfedge start : boundary_loops()) {
        Halfedge he = start;
        Vertex start_vertex = start.vertex();
        std::vector<Halfedge> loop;
        do {
            loop.push_back(he);
        } while ((he = he.next()) != start);
        for (auto he : loop) {
            // note: This HalfEdge removal code is low-level mesh editing (it is hard to maintain any invariants),
            //       and is duplicated in remove_face().
            if (!he.twin().null()) {
                he.twin().set_twin(Halfedge(*this, InvalidElementIndex));
            }
            // IMPORTANT: Remove the halfedge from the vertex_indices->halfedge map.
            // Usually, the halfedge tip can be retrieved by he.next().vertex(). However, while deleting this loop of halfedges,
            // there is a special case when removing the last halfedge, as its next() is now invalid.
            auto vertex_indices = he.next() == start ? std::pair<ElementIndex, ElementIndex>(he.vertex().index(), start_vertex.index())
                                                     : std::pair<ElementIndex, ElementIndex>(he.vertex().index(), he.next().vertex().index());
            auto found = halfedge_map.find(vertex_indices);
            assert(found != halfedge_map.end());
            halfedge_map.erase(found);
            // Remove the halfedge.
            halfedge_pool.remove(he.index());
        }
    }

    //TODO: Remove edges.

    m_locked = false;
}




std::vector<Face> SurfaceMesh::connected_components()
{
    assert(locked());
    return m_connected_components;
}

size_t SurfaceMesh::num_connected_components() const
{
    assert(locked());
    return m_connected_components.size();
}

bool SurfaceMesh::connected() const
{
    // note: 0 connected components is not considered "closed".
    return num_connected_components() == 1;
}

bool SurfaceMesh::compact() const
{
    return closed() && connected(); //---Is this a correct definition of compactness? It seems useful, though.
}


// SurfaceMesh SurfaceMesh::dual()
// {
//     assert(closed());
// 
//     SurfaceMesh dual_mesh;
// 
//     FaceAttachment<Vertex> face_to_dual_vertex(*this);
//     for (auto face : faces()) {
//         face_to_dual_vertex[face] = dual_mesh.add_vertex();
//     }
//     auto dual_face_dual_vertices = std::vector<Vertex>();
//     for (auto v : vertices()) {
//         for (auto face : v.fan()) {
//             dual_face_dual_vertices.push_back(face_to_dual_vertex[face]);
//         }
//         dual_mesh.add_face(dual_face_dual_vertices);
//         dual_face_dual_vertices.clear();
//     }
// 
//     return dual_mesh;
// }

