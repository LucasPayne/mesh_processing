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



Face Halfedge::face() const
{
    return Face(mesh, mesh.halfedge_incidence_data[*this].face_index);
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
    halfedge_incidence_data(*this),
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

// Copy assignment.
SurfaceMesh &SurfaceMesh::operator=(const SurfaceMesh &other)
{
    assert(0);
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

