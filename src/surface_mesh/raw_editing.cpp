#include "mesh_processing.h"
#include "logging.h"
#include <algorithm>//reverse

Vertex SurfaceMesh::add_vertex()
{
    auto vertex = Vertex(*this, vertex_pool.add());
    auto &vertex_incidence = vertex_incidence_data[vertex];
    vertex_incidence.halfedge_index = InvalidElementIndex;
    return vertex;
}

Halfedge SurfaceMesh::add_halfedge(Vertex u, Vertex v)
{
    assert(u != v);
    auto edge = Edge(*this, edge_pool.add());
    edge.a().set_vertex(u);
    edge.b().set_vertex(v);

    halfedge_map[std::pair<ElementIndex, ElementIndex>(u.index(), v.index())] = edge.a().index();
    halfedge_map[std::pair<ElementIndex, ElementIndex>(v.index(), u.index())] = edge.b().index();

    return edge.a();
}


static std::vector<Vertex> g_add_face_vector;
Face SurfaceMesh::add_triangle(Vertex v1, Vertex v2, Vertex v3)
{
    log("Adding triangle.");
    assert((v1 != v2) && (v2 != v3));
    g_add_face_vector.resize(3);
    g_add_face_vector[0] = v1;
    g_add_face_vector[1] = v2;
    g_add_face_vector[2] = v3;
    return add_face(g_add_face_vector);
}

Face SurfaceMesh::add_face(std::vector<Vertex> &vertices, bool been_flipped)
{
    const size_t n = vertices.size();
    log("Adding face of %zu vertices.", n);
    assert(n >= 3);
    // not checked: uniqueness of vertices.

    std::vector<bool> connected(n, false);
    std::vector<Halfedge> halfedges(n);

    // // Check for manifoldness errors.
    // for (unsigned int i = 0; i < n; i++) {
    //     //---How could this case be handled? Two connected triangles fan off of a vertex, and another disconnected one.
    //     //   another triangle is added which connects the first two into a linked neighbourhood. But now the vertex is non-manifold
    //     //   and cannot become manifold with more additions.
    //     // if (!vertices[i].is_boundary()) {
    //     //     log_error("SurfaceMesh::add_face: Vertex made non-manifold.");
    //     //     exit(1);
    //     // }
    //     auto he = get_halfedge(vertices[i], vertices[(i+1)%n]);
    //     if (!he.null()) {
    //         connected[i] = true;
    //         // if (!he.is_boundary) {
    //         //     log_error("SurfaceMesh::add_face: Edge made non-manifold.");
    //         //     exit(1);
    //         // }
    //         halfedges[i] = he;
    //     }
    // }


    /*
    vertex -> halfedge
    halfedge -> next halfedge
             -> source vertex
             -> interior face
    face -> halfedge
    */

    for (unsigned int i = 0; i < n; i++) {
        auto he = get_halfedge(vertices[i], vertices[(i+1)%n]);
        if (!he.null()) {
            printf("Halfedge already there for %u to %lu.\n", i, (i+1)%n);
            connected[i] = true;
            halfedges[i] = he;
            if (!he.face().null()) {
                log_error("Non-manifold edge.");
                // Possibly this face causes non-manifoldness because it has the wrong winding.
                // Attempt to add the face with reversed winding.
                if (!been_flipped) {
                    std::reverse(vertices.begin(), vertices.end());
                    return add_face(vertices, true);
                } else {
                    // The been_flipped parameter (defaulting to false) prevents an infinite loop.
                    return Face(*this, InvalidElementIndex);
                }
                // exit(1);
            }
        }
    }

    
    log("Adding new edges.");
    // Add new edges for unconnected successive vertices.
    for (unsigned int i = 0; i < n; i++) {
        if (!connected[i]) {
            auto he = add_halfedge(vertices[i], vertices[(i+1)%n]);
            halfedges[i] = he;
            // If this vertex is not connected to anything, link it with this halfedge.
            vertices[i].set_halfedge(he);

            // he.set_vertex(vertices[i]);
            // Set the other halfedge, the one on the boundary.
            he.flip().set_next(Halfedge(*this, InvalidElementIndex));
            he.flip().set_face(Face(*this, InvalidElementIndex));
            // he.flip().set_vertex(vertices[(i+1)%
        }
    }
    
    log("Linking halfedges.");
    // Link the halfedges to eachother.
    for (unsigned int i = 0; i < n; i++) {
        halfedges[i].set_next(halfedges[(i+1)%n]);
    }
    
    log("Adding new face.");
    // Add the face and link the halfedges to it.
    auto face = Face(*this, face_pool.add());
    face.set_halfedge(halfedges[n-1]);
    for (unsigned int i = 0; i < n; i++) {
        halfedges[i].set_face(face);
    }

    return face;
}
