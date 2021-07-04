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
        // note: vertex->halfedge incidences are not valid in an unlocked mesh. They are only computed when the mesh is locked.
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
    // ---- TODO: Verify this vertex is isolated!
    vertex_pool.remove(vertex.index());
    return true;
}

bool SurfaceMesh::remove_face(Face face)
{
    assert(!locked());
    
    // Remove this face's halfedges.
    auto start = face.halfedge();
    auto he = start;
    auto start_vertex = start.vertex(); // Needed for special case (last halfedge removal).
    do {
        if (!he.twin().null()) {
            he.twin().set_twin(Halfedge(*this, InvalidElementIndex));
        }
        auto next = he.next();

        // IMPORTANT: Remove the halfedge from the vertex_indices->halfedge map.
        // Usually, the halfedge tip can be retrieved by he.next().vertex(). However, while deleting this loop of halfedges,
        // there is a special when removing the last halfedge, as its next() is now invalid.
        auto vertex_indices = next == start ? std::pair<ElementIndex, ElementIndex>(he.vertex().index(), start_vertex.index())
                                            : std::pair<ElementIndex, ElementIndex>(he.vertex().index(), he.next().vertex().index());
        auto found = halfedge_map.find(vertex_indices);
        assert(found != halfedge_map.end());
        halfedge_map.erase(found);
        // Remove the halfedge.
        halfedge_pool.remove(he.index());

        he = next;
    } while (he != start); //note: start is removed, but the handle should still be the same when it wraps around.

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

