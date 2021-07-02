#include "mesh_processing/surface_mesh/surface_mesh.h"
#include "mesh_processing/logging.h"
#include <algorithm>//reverse


SurfaceMesh g_dummy_surface_mesh;

/*--------------------------------------------------------------------------------
    ElementPool implementations.
--------------------------------------------------------------------------------*/
ElementPool::ElementPool(size_t capacity) :
    active_flags(capacity),
    least_inactive_index{0},
    m_num_elements{0}
{
    assert(capacity > 0);
    log("Creating new element pool of capacity %zu.", capacity);
}

size_t ElementPool::capacity() const
{
    return active_flags.size();
}

void ElementPool::printout()
{
    for (bool val : active_flags) {
        printf(val ? "1" : "0");
    }
    printf(" (%zu)\n", capacity());
}


ElementIndex ElementPool::add()
{
    ElementIndex index = least_inactive_index;
    size_t n = capacity();
    for (; index < n; index++) {
        if (!is_active(index)) {
            break;
        }
    }
    assert(index != n); // note: An invariant maintained by ElementPool is that there is always at least one available entry.
    if (index == n - 1) {
        // The pool is full, grow it.
        size_t new_capacity = 2*n;
        active_flags.resize(new_capacity, 0);
        // Use the virtual resize() method to resize the attachments in the same way.
        // These new attachment entries will be value initialized, which uses the default constructor if available.
        for (auto attachment : attachments) {
            attachment->resize(new_capacity);
        }
    }
    for (auto attachment : attachments) {
        // Use the virtual create() method to create the default entry.
        attachment->create(index);
    }
    active_flags[index] = true;
    least_inactive_index = index+1;

    m_num_elements += 1; // update the cached element count.
    return index;
}


void ElementPool::remove(ElementIndex element_index)
{
    if (element_index < least_inactive_index) {
        least_inactive_index = element_index;
    }
    // Use the virtual destroy() method to tear down the entry.
    for (auto attachment : attachments) {
        attachment->destroy(element_index);
    }
    m_num_elements -= 1; // update the cached element count.
}

ElementPoolIterator ElementPool::begin()
{
    return ElementPoolIterator(this, 0);
}
ElementPoolIterator ElementPool::end()
{
    return ElementPoolIterator(this, InvalidElementIndex);
}





/*--------------------------------------------------------------------------------
    ElementPoolIterator
--------------------------------------------------------------------------------*/
ElementPoolIterator::ElementPoolIterator(ElementPool *_element_pool, ElementIndex _element_index) :
    element_pool{_element_pool},
    element_index{_element_index},
    n{_element_pool->capacity()}
{
    // Set up the beginning index (the first active entry in the pool).
    if (element_index != InvalidElementIndex) {
        ElementIndex i = element_index;
        for (; i < n; i++) {
            if (element_pool->is_active(i)) {
                element_index = i;
                break;
            }
        }
        if (i == n) element_index = InvalidElementIndex;
    }
}
ElementIndex ElementPoolIterator::operator*() const
{
    return element_index;
}
ElementPoolIterator &ElementPoolIterator::operator++()
{
    for (ElementIndex i = element_index+1; i < n; i++) {
        if (element_pool->is_active(i)) {
            element_index = i;
            return *this;
        }
    }
    element_index = InvalidElementIndex;
    return *this;
}

// Equality/inequality assumes that the element pools are the same,
// which is probably reasonable.
bool ElementPoolIterator::operator==(const ElementPoolIterator &other) const
{
    return element_index == other.element_index;
}
bool ElementPoolIterator::operator!=(const ElementPoolIterator &other) const
{
    return !operator==(other);
}




/*--------------------------------------------------------------------------------
    ElementHandle
--------------------------------------------------------------------------------*/

ElementHandle::ElementHandle(SurfaceMesh &_mesh, ElementIndex _index) :
    mesh{_mesh}, m_index{_index}
{}

// Copy assignment.
ElementHandle &ElementHandle::operator=(const ElementHandle &other) {
    //----If buggy, check this!
    // Placement-new is used here so the mesh reference member can be copied.
    new(this) ElementHandle(other);
    return *this;
}


ElementAttachmentBase::ElementAttachmentBase(size_t _type_size) :
    type_size{_type_size}, raw_data{nullptr}
{}




Halfedge Vertex::halfedge() const
{
    return Halfedge(mesh, mesh.vertex_incidence_data[*this].halfedge_index);
}
void Vertex::set_halfedge(Halfedge halfedge)
{
    mesh.vertex_incidence_data[*this].halfedge_index = halfedge.index();
}



Halfedge Edge::a() const
{
    return Halfedge(mesh, 2*m_index);
}
Halfedge Edge::b() const
{
    return Halfedge(mesh, 2*m_index + 1);
}
Halfedge Edge::halfedge(int index) const
{
    assert(index == 0 || index == 1);
    return Halfedge(mesh, 2*m_index + index);
}

Halfedge Halfedge::next() const
{
    return Halfedge(mesh, mesh.edge_incidence_data[*this].next_index);
}
Vertex Halfedge::vertex() const
{
    return Vertex(mesh, mesh.edge_incidence_data[*this].vertex_index);
}
Vertex Halfedge::tip() const
{
    return flip().vertex();
}

Halfedge Halfedge::flip() const
{
    // printf("Flipping %u to %u\n", m_index, m_index ^ 1);getchar();
    return Halfedge(mesh, m_index ^ 1);
}
void Halfedge::set_vertex(Vertex vertex)
{
    mesh.edge_incidence_data[*this].vertex_index = vertex.index();
}
void Halfedge::set_face(Face face)
{
    mesh.edge_incidence_data[*this].face_index = face.index();
}
void Halfedge::set_next(Halfedge halfedge)
{
    mesh.edge_incidence_data[*this].next_index = halfedge.index();
}

bool Halfedge::is_boundary()
{
    return flip().face().null();
}



Face Halfedge::face() const
{
    return Face(mesh, mesh.edge_incidence_data[*this].face_index);
}

Halfedge Face::halfedge() const
{
    return Halfedge(mesh, mesh.face_incidence_data[*this].halfedge_index);
}
void Face::set_halfedge(Halfedge halfedge)
{
    mesh.face_incidence_data[*this].halfedge_index = halfedge.index();
}
int Face::num_vertices()
{
    if (null()) return 0;
    int i = 0;
    auto start = halfedge();
    auto he = start;
    do {
        i++;
        he = he.next();
    } while (he != start);
    return i;
}


/*--------------------------------------------------------------------------------
    SurfaceMesh
--------------------------------------------------------------------------------*/
SurfaceMesh::SurfaceMesh() :
    vertex_incidence_data(*this),
    edge_incidence_data(*this),
    face_incidence_data(*this)
{
    log("Creating surface mesh.");
}


Vertex SurfaceMesh::add_vertex()
{
    log("Adding vertex.");
    auto vertex = Vertex(*this, vertex_pool.add());
    auto &vertex_incidence = vertex_incidence_data[vertex];
    vertex_incidence.halfedge_index = InvalidElementIndex;
    return vertex;
}

Halfedge SurfaceMesh::add_halfedge(Vertex u, Vertex v)
{
    auto edge = Edge(*this, edge_pool.add());
    // This is the only connectivity information known by this method.
    // The caller must set up the rest of the incidence relations.
    // printf("%u %u %u\n", edge.index(), edge.a().index(), edge.b().index());
    edge.a().set_vertex(u);
    edge.b().set_vertex(v);

    halfedge_map[std::pair<ElementIndex, ElementIndex>(u.index(), v.index())] = edge.a().index();
    halfedge_map[std::pair<ElementIndex, ElementIndex>(v.index(), u.index())] = edge.b().index();

    return edge.a();
}
Halfedge SurfaceMesh::get_halfedge(Vertex u, Vertex v)
{
    auto found = halfedge_map.find(std::pair<ElementIndex, ElementIndex>(u.index(), v.index()));
    if (found == halfedge_map.end()) {
        return Halfedge(*this, InvalidElementIndex);
    }
    return Halfedge(*this, found->second);
    // auto start = u.halfedge();
    // auto he = start;
    // while (!he.null()) {
    //     he = he.flip().next();
    //     if (he == start) break;
    // }
    // return he;
}

size_t SurfaceMesh::num_vertices() const
{
    return vertex_pool.num_elements();
}
size_t SurfaceMesh::num_edges() const
{
    return edge_pool.num_elements();
}
size_t SurfaceMesh::num_faces() const
{
    return face_pool.num_elements();
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


void SurfaceMesh::printout()
{
    printf("============================================================\n");
    printf("SurfaceMesh printout\n");
    printf("------------------------------------------------------------\n");
    printf("vertex_pool\n");
    vertex_pool.printout();
    printf("edge_pool\n");
    edge_pool.printout();
    printf("face_pool\n");
    face_pool.printout();
    printf("============================================================\n");
}



