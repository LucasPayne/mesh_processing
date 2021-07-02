#ifndef SURFACE_MESH_H
#define SURFACE_MESH_H
#include "mesh_processing/core.h"
#include <utility>
#include <map>


// typedefs
typedef uint32_t ElementIndex;
constexpr ElementIndex InvalidElementIndex = std::numeric_limits<ElementIndex>::max();

// forward declarations
class SurfaceMesh;
class ElementPool;
template <typename T> class ElementAttachment;
class ElementAttachmentBase;
class Vertex;
class Halfedge;
class Edge;
class Face;



/*--------------------------------------------------------------------------------
    ElementPool
--------------------------------------------------------------------------------*/

class ElementPoolIterator {
public:
    ElementPoolIterator(ElementPool *_element_pool = nullptr, ElementIndex _element_index = InvalidElementIndex);

    ElementIndex operator*() const;
    ElementPoolIterator &operator++();
    bool operator==(const ElementPoolIterator &other) const;
    bool operator!=(const ElementPoolIterator &other) const;
private:
    ElementPool *element_pool;
    ElementIndex element_index;
    size_t n;
};


class ElementPool {
public:
    ElementPool(size_t capacity = 1);

    size_t capacity() const;
    ElementIndex add();
    void remove(ElementIndex element_index);

    inline bool is_active(ElementIndex element_index) const {
        return active_flags[element_index];
    }
    inline size_t num_elements() const { return m_num_elements; }

    void printout();

    ElementPoolIterator begin();
    ElementPoolIterator end();
    
private:
    std::vector<bool> active_flags;
    std::vector<ElementAttachmentBase *> attachments;

    // implementation details.
    ElementIndex least_inactive_index;

    size_t m_num_elements; // This must be kept up-to-date.

    template <typename T>
    friend class ElementAttachment;
};


/*--------------------------------------------------------------------------------
    ElementAttachmentBase and ElementAttachment<T>
--------------------------------------------------------------------------------*/
class ElementAttachmentBase {
protected:
    ElementAttachmentBase(size_t _type_size);
    size_t type_size;
    uint8_t *raw_data;
    
private:
    // Virtual shadowing methods. These provide a generic interface to update the templated ElementAttachment's vector<T>
    // to match the layout of the ElementPool.
    virtual void resize(size_t n) = 0;
    virtual void create(ElementIndex element_index) = 0;
    virtual void destroy(ElementIndex element_index) = 0;

    friend class ElementPool; // ElementPool needs access to the virtual shadowing methods.
};


template <typename T>
class ElementAttachment : public ElementAttachmentBase {
public:
    ~ElementAttachment();
protected:
    ElementAttachment(ElementPool &_pool);
    T &get(ElementIndex element_index); // [] overload isn't used here since the only users are derived classes.
private:
    virtual void resize(size_t n) final;
    virtual void create(ElementIndex element_index) final;
    virtual void destroy(ElementIndex element_index) final;

    ElementPool &pool;
    std::vector<T> data;
};



/*--------------------------------------------------------------------------------
    Vertex, edge, and face attachments.
--------------------------------------------------------------------------------*/
template <typename T>
class VertexAttachment : public ElementAttachment<T> {
public:
    VertexAttachment(SurfaceMesh &mesh);
    T &operator[](const Vertex &vertex);
};



template <typename T>
struct HalfedgeDataPair {
    T halfedges[2];
};
template <typename T>
class EdgeAttachment : public ElementAttachment<HalfedgeDataPair<T>> {
public:
    EdgeAttachment(SurfaceMesh &mesh);
    // Edge handles access to pairs of the T data, one for each associated halfedge.
    HalfedgeDataPair<T> &operator[](const Edge &edge);
    // Halfedge handles access the template parameter type T.
    T &operator[](const Halfedge &halfedge);
};


template <typename T>
class FaceAttachment : public ElementAttachment<T> {
public:
    FaceAttachment(SurfaceMesh &mesh);
    T &operator[](const Face &face);
};






struct VertexIncidenceData {
    // A halfedge index is 2*edge_index + subindex, where subindex is 0 or 1 according to the choice of halfedge.
    ElementIndex halfedge_index;
};
struct FaceIncidenceData {
    ElementIndex halfedge_index;
};
struct HalfedgeIncidenceData {
    ElementIndex next_index;
    ElementIndex vertex_index;
    ElementIndex face_index;
};



class ElementHandle {
public:
    inline ElementIndex index() const { return m_index; }

    // For efficiency reasons, the equality operator does not check if these elements are of the same mesh.
    inline bool operator==(const ElementHandle &other) const { return m_index == other.m_index; }
    inline bool operator!=(const ElementHandle &other) const { return !(*this == other); }

    ElementHandle &operator=(const ElementHandle &other);

    inline bool null() const { return m_index == InvalidElementIndex; }
protected:
    ElementHandle(SurfaceMesh &_mesh, ElementIndex _index);
    SurfaceMesh &mesh;
    ElementIndex m_index;
    friend class SurfaceMesh;
};


extern SurfaceMesh g_dummy_surface_mesh;
class Vertex : public ElementHandle {
public:
    Halfedge halfedge() const;
    Vertex(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}
    Vertex() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}

    // Returns true if this vertex has no cycle among its neighbours.
    // bool is_boundary();

private:
    void set_halfedge(Halfedge halfedge);

    friend class SurfaceMesh;
};

class Edge : public ElementHandle {
public:
    Halfedge a() const;
    Halfedge b() const;
    Halfedge halfedge(int index) const;
    Edge(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}
    Edge() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}
private:
    friend class SurfaceMesh;
};


class Halfedge : public ElementHandle {
public:
    Halfedge next() const;
    Halfedge flip() const;
    Vertex vertex() const;
    Vertex tip() const;
    Face face() const;
    bool is_boundary();
    Halfedge(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}
    Halfedge() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}
private:
    void set_vertex(Vertex vertex);
    void set_face(Face face);
    void set_next(Halfedge halfedge);
    friend class SurfaceMesh;
};


class Face : public ElementHandle {
public:
    Halfedge halfedge() const;
    int num_vertices();

    Face(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}
    Face() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}
private:
    void set_halfedge(Halfedge halfedge);
    friend class SurfaceMesh;
};



template <typename T>
class ElementIterator {
public:
    ElementIterator(SurfaceMesh *_mesh, ElementPool *_element_pool, ElementIndex _element_index);
    T operator*();
    ElementIterator<T> &operator++();
    bool operator==(const ElementIterator &other) const;
    bool operator!=(const ElementIterator &other) const;

private:
    SurfaceMesh *mesh;
    ElementPoolIterator element_pool_iterator;
};



class SurfaceMesh {
public:
    SurfaceMesh();

    
    // Creation methods.
    // These do not necessarily maintain invariants.
    Vertex add_vertex();
    Face add_triangle(Vertex v1, Vertex v2, Vertex v3);
    Face add_face(std::vector<Vertex> &vertices, bool been_flipped = false);

    Halfedge add_halfedge(Vertex u, Vertex v);
    Halfedge get_halfedge(Vertex u, Vertex v);

    // Counting elements.
    size_t num_vertices() const;
    size_t num_edges() const;
    size_t num_faces() const;

    void printout();

    
    template <typename T>
    class ElementContainer {
    public:
        ElementContainer(SurfaceMesh *_mesh, ElementPool *_element_pool) :
            mesh{_mesh},
            element_pool{_element_pool}
        {}
        ElementIterator<T> begin() { return ElementIterator<T>(mesh, element_pool, 0); }
        ElementIterator<T> end() { return ElementIterator<T>(mesh, element_pool, InvalidElementIndex); }
    private:
        SurfaceMesh *mesh;
        ElementPool *element_pool;
    };
    ElementContainer<Vertex> vertices() {
        return ElementContainer<Vertex>(this, &vertex_pool);
    }
    ElementContainer<Edge> edges() {
        return ElementContainer<Edge>(this, &edge_pool);
    }
    ElementContainer<Face> faces() {
        return ElementContainer<Face>(this, &face_pool);
    }


private:
    ElementPool vertex_pool;
    ElementPool edge_pool;
    ElementPool face_pool;
    VertexAttachment<VertexIncidenceData> vertex_incidence_data;
    EdgeAttachment<HalfedgeIncidenceData> edge_incidence_data;
    FaceAttachment<FaceIncidenceData> face_incidence_data;

    std::map<std::pair<ElementIndex, ElementIndex>, ElementIndex> halfedge_map; //vertices to halfedge.


    template <typename T>
    friend class ElementAttachment;
    template <typename T>
    friend class VertexAttachment;
    template <typename T>
    friend class EdgeAttachment;
    template <typename T>
    friend class FaceAttachment;

    friend class Vertex;
    friend class Edge;
    friend class Halfedge;
    friend class Face;

    friend class ElementIterator<Vertex>;
    friend class ElementIterator<Edge>;
    friend class ElementIterator<Face>;
};




#include "surface_mesh.ipp"

#endif // SURFACE_MESH_H
