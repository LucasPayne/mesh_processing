#include "mesh_processing/mesh_processing.h"
#include <algorithm>//reverse

Vertex SurfaceMesh::add_vertex()
{
    assert(!locked());
    auto vertex = Vertex(*this, vertex_pool.add());
    auto &vertex_incidence = vertex_incidence_data[vertex];
    vertex_incidence.halfedge_index = InvalidElementIndex;
    return vertex;
}

Face SurfaceMesh::add_triangle(Vertex v1, Vertex v2, Vertex v3)
{
    Vertex vertices[3] = {v1, v2, v3};
    return add_face(vertices, 3);
}

Face SurfaceMesh::add_face(std::vector<Vertex> &vertices)
{
    return add_face(&vertices[0], vertices.size());
}

Face SurfaceMesh::add_face(Vertex *vertices, int num_vertices)
{
    #define MAX_NUM_VERTICES 20 // Set a maximum number of vertices so dynamic memory doesn't need to be used.
    assert(!locked());
    assert(num_vertices <= MAX_NUM_VERTICES);

    // Create a loop of halfedges around this face, and set up the vertex and halfedge incidence information.
    Halfedge halfedges[MAX_NUM_VERTICES];
    for (int i = 0; i < num_vertices; i++) {
        Vertex &u = vertices[i];
        Vertex &v = vertices[(i+1)%num_vertices];

        // Check that there is not already a halfedge u->v.
        assert(get_halfedge(u, v).null());
        // Create a new halfedge.
        auto he = Halfedge(*this, halfedge_pool.add());
        halfedges[i] = he;
        // Add this to the halfedge_map, which is used to quickly find halfedges between two vertices.
        halfedge_map[std::pair<ElementIndex,ElementIndex>(u.index(), v.index())] = he.index();

        // Update halfedge twin references.
        auto twin_he = get_halfedge(v, u);
        he.set_twin(twin_he);
        if (!twin_he.null()) {
            twin_he.set_twin(he);
        }

        // Attach to source vertex.
        he.set_vertex(u);
        u.set_halfedge(he); //todo: maybe need to think about the boundary case
    }
    // Connect the halfedges up into a loop.
    for (int i = 0; i < num_vertices; i++) {
        halfedges[i].set_next(halfedges[(i+1)%num_vertices]);
    }
    
    // Create the interior face, and connect it to the halfedges.
    auto face = Face(*this, face_pool.add());
    for (int i = 0; i < num_vertices; i++) {
        halfedges[i].set_face(face);
    }
    face.set_halfedge(halfedges[0]);

    return face;
    #undef MAX_NUM_VERTICES
}


bool SurfaceMesh::remove_vertex(Vertex vertex)
{
    assert(!locked());
    // Can only remove an isolated vertex.
    assert(vertex.halfedge().null());
    vertex_pool.remove(vertex.index());
    return true;
}

bool SurfaceMesh::remove_face(Face face)
{
    assert(!locked());
    
    // Make sure that vertices do not reference the halfedges that will be removed.
    auto he = face.halfedge();
    do {
        if (he.vertex().halfedge() == he) {
            if (!he.twin().next().null()) {
                
            }
        }
    } while ((he = he.next()) != face.halfedge());

    // // Remove this face's halfedges.
    // auto start = face.halfedge();
    // auto he = start;
    // do {
    //     if (!he.twin().null()) {
    //         he.twin().set_twin(Halfedge(*this, InvalidElementIndex));
    //     }
    //     auto next = he.next();
    //     halfedge_pool.remove(he.index());
    //     he = next;
    // } while (he != start); //note: start is removed, but the handle should still be the same when it wraps around.

    face_pool.remove(face.index());

    return true;
}


// private methods
//--------------------------------------------------------------------------------
Halfedge SurfaceMesh::get_halfedge(Vertex u, Vertex v)
{
    auto found = halfedge_map.find(std::pair<ElementIndex, ElementIndex>(u.index(), v.index()));
    if (found == halfedge_map.end()) {
        return Halfedge(*this, InvalidElementIndex);
    }
    return Halfedge(*this, found->second);
}

