#include "mesh_processing/mesh_processing.h"


/* Add all mesh elements of another mesh, disjointly, to this mesh.
 */
void SurfaceMesh::add(SurfaceMesh &mesh)
{
    // assert(locked());
    //---lock?
    unlock();
    
    // To add the mesh, create a new vertex (in this mesh) for each vertex in the added mesh.
    // Keep a vertex attachment on the added mesh, which will contain the raw indices in the list new_vertices.
    auto vertex_indices = VertexAttachment<uint32_t>(mesh);
    auto new_vertices = std::vector<Vertex>(mesh.num_vertices());
    uint32_t vertex_index = 0;
    for (auto v : mesh.vertices()) {
        vertex_indices[v] = vertex_index;
        new_vertices[vertex_index] = add_vertex();
        vertex_index += 1;
    }
    
    // To add faces, go through each face in the added mesh,
    // and iterate over its vertices. Look up the raw index in the vertex attachment
    // to get a Vertex reference for the corresponding new vertex. A list of these
    // new vertices is formed to create a new face.
    auto face_vertices = std::vector<Vertex>();
    for (auto face : mesh.faces()) {
        auto start = face.halfedge();
        auto he = start;
        do {
            face_vertices.push_back(new_vertices[vertex_indices[he.vertex()]]);
        } while ((he = he.next()) != start);
        add_face(face_vertices);
        face_vertices.clear();
    }
    
    // lock();
}

void SurfaceMesh::remove_connected_component(Face starting_face)
{
    assert(locked());

    FaceAttachment<char> visited(*this);
    for (auto face : faces()) visited[face] = false;
    VertexAttachment<char> vertex_visited(*this);
    for (auto v : vertices()) vertex_visited[v] = false;

    auto faces_to_remove = std::vector<Face>();
    auto vertices_to_remove = std::vector<Vertex>();

    std::function<void(Face)> search = [&](Face face) {
	visited[face] = true;
        faces_to_remove.push_back(face);
        auto start = face.halfedge();
        auto he = start;
        do {
            assert(!he.twin().null());
            if (!vertex_visited[he.vertex()]) {
                vertex_visited[he.vertex()] = true;
                vertices_to_remove.push_back(he.vertex());
            }
            if (!he.twin().face().null() && !visited[he.twin().face()]) {
                search(he.twin().face());
            }
        } while ((he = he.next()) != start);
    };
    search(starting_face);

    unlock();
    for (auto face : faces_to_remove) {
        remove_face(face);
    }
    for (auto v : vertices_to_remove) {
        remove_vertex(v);
    }
    lock();
}


bool SurfaceMesh::is_triangular()
{
    for (auto face : faces()) {
        if (face.num_vertices() != 3) return false;
    }
    return true;
}

bool SurfaceMesh::is_quad()
{
    for (auto face : faces()) {
        if (face.num_vertices() != 4) return false;
    }
    return true;
}




