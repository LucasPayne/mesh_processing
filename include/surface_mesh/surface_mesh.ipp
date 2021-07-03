/*--------------------------------------------------------------------------------
surface_mesh.ipp
    Template method implementations.
--------------------------------------------------------------------------------*/ 


/*--------------------------------------------------------------------------------
    ElementAttachment template methods.
--------------------------------------------------------------------------------*/
// Constructor
template <typename T>
ElementAttachment<T>::ElementAttachment(ElementPool &_pool) :
    ElementAttachmentBase(sizeof(T)),
    pool{_pool},
    data(_pool.capacity())
{
    // Set the base class's uint8_t pointer.
    raw_data = reinterpret_cast<uint8_t *>(&data[0]);

    pool.attachments.push_back(this);
}


// Destructor
template <typename T>
ElementAttachment<T>::~ElementAttachment()
{
    pool.attachments.erase(std::find(pool.attachments.begin(), pool.attachments.end(), this));
}


// Accessor.
template <typename T>
T &ElementAttachment<T>::get(ElementIndex element_index)
{
    if (!pool.is_active(element_index)) {
        assert(pool.is_active(element_index));
    }
    return data[element_index];
}


// Virtual methods for shadowing the ElementPool.
template <typename T>
void ElementAttachment<T>::resize(size_t n)
{
    data.resize(n);
}


template <typename T>
void ElementAttachment<T>::create(ElementIndex element_index)
{
    // Default-initialize the entry.
    new (&data[element_index]) T;
}


template <typename T>
void ElementAttachment<T>::destroy(ElementIndex element_index)
{
    // Destroy the entry.
    data[element_index].~T();
}



/*--------------------------------------------------------------------------------
    Vertex, Halfedge, and Face element attachment template methods.
--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------
    VertexAttachment
--------------------------------------------------------------------------------*/
template <typename T>
VertexAttachment<T>::VertexAttachment(SurfaceMesh &mesh) :
    ElementAttachment<T>(mesh.vertex_pool)
{}

template <typename T>
T &VertexAttachment<T>::operator[](const Vertex &vertex)
{
    return this->get(vertex.index());
}


/*--------------------------------------------------------------------------------
    HalfedgeAttachment
--------------------------------------------------------------------------------*/
template <typename T>
HalfedgeAttachment<T>::HalfedgeAttachment(SurfaceMesh &mesh) :
    ElementAttachment<T>(mesh.halfedge_pool)
{}

template <typename T>
T &HalfedgeAttachment<T>::operator[](const Halfedge &halfedge)
{
    return this->get(halfedge.index());
}


/*--------------------------------------------------------------------------------
    FaceAttachment
--------------------------------------------------------------------------------*/
template <typename T>
FaceAttachment<T>::FaceAttachment(SurfaceMesh &mesh) :
    ElementAttachment<T>(mesh.face_pool)
{}

template <typename T>
T &FaceAttachment<T>::operator[](const Face &face)
{
    return this->get(face.index());
}


/*--------------------------------------------------------------------------------
    Element iterators.
These iterators are simple wrappers to ElementPoolIterator that augment the iterated type.
--------------------------------------------------------------------------------*/
template <typename T>
ElementIterator<T>::ElementIterator(SurfaceMesh *_mesh, ElementPool *_element_pool, ElementIndex _element_index) :
    mesh{_mesh},
    element_pool_iterator(_element_pool, _element_index)
{}

template <typename T>
T ElementIterator<T>::operator*()
{
    return T(*mesh, *element_pool_iterator);
}

template <typename T>
ElementIterator<T> &ElementIterator<T>::operator++()
{
    ++element_pool_iterator;
    return *this;
}

template <typename T>
bool ElementIterator<T>::operator==(const ElementIterator &other) const
{
    return element_pool_iterator == other.element_pool_iterator;
}

template <typename T>
bool ElementIterator<T>::operator!=(const ElementIterator &other) const
{
    return !operator==(other);
}

