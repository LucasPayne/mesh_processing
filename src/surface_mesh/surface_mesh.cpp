#include "mesh_processing/mesh_processing.h"
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
    assert(is_active(element_index)); // Can only remove elements that are actually there.
    if (element_index < least_inactive_index) {
        least_inactive_index = element_index;
    }
    // Use the virtual destroy() method to tear down the entry.
    for (auto attachment : attachments) {
        attachment->destroy(element_index);
    }
    active_flags[element_index] = false;
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




void Vertex::set_halfedge(Halfedge halfedge)
{
    mesh.vertex_incidence_data[*this].halfedge_index = halfedge.index();
}

void Halfedge::set_vertex(Vertex vertex)
{
    mesh.halfedge_incidence_data[*this].vertex_index = vertex.index();
}
void Halfedge::set_face(Face face)
{
    mesh.halfedge_incidence_data[*this].face_index = face.index();
}
void Halfedge::set_next(Halfedge halfedge)
{
    mesh.halfedge_incidence_data[*this].next_index = halfedge.index();
}
void Halfedge::set_twin(Halfedge halfedge)
{
    mesh.halfedge_incidence_data[*this].twin_index = halfedge.index();
}
void Halfedge::set_edge(Edge edge)
{
    mesh.halfedge_incidence_data[*this].edge_index = edge.index();
}

void Edge::set_halfedge_a(Halfedge halfedge)
{
    mesh.edge_incidence_data[*this].halfedge_indices[0] = halfedge.index();
}
void Edge::set_halfedge_b(Halfedge halfedge)
{
    mesh.edge_incidence_data[*this].halfedge_indices[1] = halfedge.index();
}




Halfedge Vertex::halfedge() const
{
    // Not valid when mesh is unlocked.
    //   (Unlocked meshes don't have full half-edge data, so vertex->halfedge relations don't give full information for traversal.
    //    It is simpler to just not expect valid vertex incidences, and set them up only when the mesh is locked.)
    if (!mesh.locked()) {
        std::cerr << "mesh traversal error: vertex->halfedge traversal is only valid when the mesh is locked.\n";
        exit(EXIT_FAILURE);
    }
    return Halfedge(mesh, mesh.vertex_incidence_data[*this].halfedge_index);
}
size_t Vertex::num_adjacent_vertices() const
{
    if (!mesh.locked()) {
        std::cerr << "mesh traversal error: Vertex::num_adjacent_vertices() is only valid when the mesh is locked.\n";
        exit(EXIT_FAILURE);
    }
    auto start = halfedge();
    auto he = start;
    int n = 0;
    do {
        n++;
    } while ((he = he.twin().next()) != start);
    return n;
}

Halfedge Halfedge::next() const
{
    return Halfedge(mesh, mesh.halfedge_incidence_data[*this].next_index);
}
Halfedge Halfedge::twin() const {
    return Halfedge(mesh, mesh.halfedge_incidence_data[*this].twin_index);
}

Vertex Halfedge::vertex() const
{
    return Vertex(mesh, mesh.halfedge_incidence_data[*this].vertex_index);
}
Vertex Halfedge::tip() const
{
    return next().vertex();
}

Face Halfedge::face() const
{
    return Face(mesh, mesh.halfedge_incidence_data[*this].face_index);
}
Edge Halfedge::edge() const
{
    if (!mesh.locked()) {
        std::cerr << "mesh traversal error: halfedge->edge traversal is only valid when the mesh is locked.\n";
        exit(EXIT_FAILURE);
    }
    return Edge(mesh, mesh.halfedge_incidence_data[*this].edge_index);
}

Halfedge Face::halfedge() const
{
    return Halfedge(mesh, mesh.face_incidence_data[*this].halfedge_index);
}
void Face::set_halfedge(Halfedge halfedge)
{
    mesh.face_incidence_data[*this].halfedge_index = halfedge.index();
}
size_t Face::num_vertices() const
{
    auto start = halfedge();
    auto he = start;
    int n = 0;
    do {
        n++;
    } while ((he = he.next()) != start);
    return n;
}

Halfedge Edge::a() const
{
    return Halfedge(mesh, mesh.edge_incidence_data[*this].halfedge_indices[0]);
}
Halfedge Edge::b() const
{
    return Halfedge(mesh, mesh.edge_incidence_data[*this].halfedge_indices[1]);
}


/*--------------------------------------------------------------------------------
    SurfaceMesh
--------------------------------------------------------------------------------*/
SurfaceMesh::SurfaceMesh() :
    vertex_incidence_data(*this),
    halfedge_incidence_data(*this),
    edge_incidence_data(*this),
    face_incidence_data(*this),
    m_locked{false}
{
}

size_t SurfaceMesh::num_vertices() const
{
    return vertex_pool.num_elements();
}
size_t SurfaceMesh::num_halfedges() const
{
    return halfedge_pool.num_elements();
}
size_t SurfaceMesh::num_faces() const
{
    return face_pool.num_elements();
}
size_t SurfaceMesh::num_edges() const
{
    // The idea of "edge" is only valid when the mesh is locked.
    assert(locked() && num_halfedges() % 2 == 0);
    return num_halfedges() / 2;
}

// Copy assignment.
SurfaceMesh &SurfaceMesh::operator=(const SurfaceMesh &other)
{
    assert(0); //todo: Implement this.
    return *this;
}




void SurfaceMesh::printout()
{
    printf("============================================================\n");
    printf("SurfaceMesh printout\n");
    printf("------------------------------------------------------------\n");
    printf("vertex_pool\n");
    vertex_pool.printout();
    printf("halfedge_pool\n");
    halfedge_pool.printout();
    printf("face_pool\n");
    face_pool.printout();
    printf("============================================================\n");
}

