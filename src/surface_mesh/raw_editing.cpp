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

Face SurfaceMesh::add_triangle(Vertex v1, Vertex v2, Vertex v3)
{
    log("Adding triangle.");
    Vertex vertices[3] = {v1, v2, v3};

    // Create a loop of halfedges around this face, and set up the vertex and halfedge incidence information.
    Halfedge halfedges[3];
    for (int i = 0; i < 3; i++) {
        Vertex &u = vertices[i];
        Vertex &v = vertices[(i+1)%3];

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
    for (int i = 0; i < 3; i++) {
        halfedges[i].set_next(halfedges[(i+1)%3]);
    }
    
    // Create the interior face, and connect it to the halfedges.
    auto face = Face(*this, face_pool.add());
    for (int i = 0; i < 3; i++) {
        halfedges[i].set_face(face);
    }
    face.set_halfedge(halfedges[0]);
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

