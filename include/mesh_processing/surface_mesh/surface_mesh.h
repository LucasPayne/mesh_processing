#ifndef SURFACE_MESH_H
#define SURFACE_MESH_H
#include <utility>
#include <map>
#include <assert.h>


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
    const T &get(ElementIndex element_index) const;
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
    const T &operator[](const Vertex &vertex) const;
    T &operator[](const Vertex &vertex);
};


template <typename T>
class HalfedgeAttachment : public ElementAttachment<T> {
public:
    HalfedgeAttachment(SurfaceMesh &mesh);
    const T &operator[](const Halfedge &halfedge) const;
    T &operator[](const Halfedge &halfedge);
};


template <typename T>
class EdgeAttachment : public ElementAttachment<T> {
public:
    EdgeAttachment(SurfaceMesh &mesh);
    const T &operator[](const Edge &edge) const;
    T &operator[](const Edge &edge);
};


template <typename T>
class FaceAttachment : public ElementAttachment<T> {
public:
    FaceAttachment(SurfaceMesh &mesh);
    const T &operator[](const Face &face) const;
    T &operator[](const Face &face);
};






struct VertexIncidenceData {
    ElementIndex halfedge_index;
};
struct FaceIncidenceData {
    ElementIndex halfedge_index;
};
struct HalfedgeIncidenceData {
    ElementIndex next_index;
    ElementIndex vertex_index;
    ElementIndex face_index;
    ElementIndex twin_index;
    ElementIndex edge_index;
};
struct EdgeIncidenceData {
    ElementIndex halfedge_indices[2];
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
    Halfedge halfedge() const; // Only valid when the mesh is locked (single vertex->halfedge relations don't give full traversals for unlocked (non-manifold) meshes.)
    size_t num_adjacent_vertices() const; // Only valid when mesh is locked.
    Vertex() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}

    bool on_boundary() const; // Only valid when mesh is locked.

    //todo: Hide from public interface.
    Vertex(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}

private:
    void set_halfedge(Halfedge halfedge);

    friend class SurfaceMesh;
};


class Halfedge : public ElementHandle {
public:
    Halfedge next() const;
    Halfedge twin() const;
    Vertex vertex() const;
    Vertex tip() const;
    Face face() const;
    Edge edge() const;

    Halfedge() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}
    //todo: Hide from public interface.
    Halfedge(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}
private:
    void set_vertex(Vertex vertex);
    void set_face(Face face);
    void set_next(Halfedge halfedge);
    void set_twin(Halfedge halfedge);
    void set_edge(Edge edge);

    friend class SurfaceMesh;
};


class Face : public ElementHandle {
public:
    Halfedge halfedge() const;
    size_t num_vertices() const;

    Face() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}
    //todo: Hide from public interface.
    Face(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}
private:
    void set_halfedge(Halfedge halfedge);
    friend class SurfaceMesh;
};

// note: Edges only make sense when the mesh is locked (all halfedges have a twin).
class Edge : public ElementHandle {
public:
    Halfedge a() const;
    Halfedge b() const;

    inline bool on_boundary() const {
        return a().face().null() || b().face().null();
    };

    Edge() :
        ElementHandle(g_dummy_surface_mesh, InvalidElementIndex)
    {}
    //todo: Hide from public interface.
    Edge(SurfaceMesh &_mesh, ElementIndex _index) :
        ElementHandle(_mesh, _index)
    {}
private:
    void set_halfedge_a(Halfedge halfedge);
    void set_halfedge_b(Halfedge halfedge);
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


/* Important invariants:
 *    If nothing is deleted from a surface mesh, then elements must be contiguous. The ability to make this assumption is useful (e.g. when loading from
 *    a triangle list).
 */
class SurfaceMesh {
public:
    SurfaceMesh();

    // Copy assignment.
    SurfaceMesh &operator=(const SurfaceMesh &other);

    // Raw editing methods.
    // These do not necessarily maintain invariants, and are only valid when the mesh is unlocked.
    Vertex add_vertex();
    Face add_triangle(Vertex v1, Vertex v2, Vertex v3);
    Face add_face(Vertex *vertices, int num_vertices);
    Face add_face(std::vector<Vertex> &vertices);

    bool remove_vertex(Vertex vertex); // Remove an isolated vertex.
    bool remove_face(Face face); // Remove a face and all its halfedges. Vertices are unaffected.

    // Euler editing methods.
    // These maintain manifoldness, and are only valid when the mesh is locked.
    void add(SurfaceMesh &mesh);
    void remove_connected_component(Face starting_face);

    // Topology.
    bool locked() const;
    void lock();
    void unlock();
    // Boundary.
    std::vector<Halfedge> boundary_loops();
    size_t num_boundary_loops() const;
    // Connected components.
    std::vector<Face> connected_components(); // Returns one face from each connected component.
    size_t num_connected_components() const;
    // Manifold properties.
    bool closed() const;
    bool connected() const;
    bool compact() const; // A compact mesh is compact as a manifold (is both closed/boundaryless and connected).
    // Tests for specific type of mesh.
    bool is_triangular(); //todo: Make this const.
    bool is_quad();

    // Counting elements.
    size_t num_vertices() const;
    size_t num_interior_vertices() const;
    size_t num_halfedges() const;
    size_t num_faces() const;
    size_t num_edges() const;
    size_t num_interior_edges() const;

    // Accessors.
    // If there is an edge, this returns the edge between vertex a and b.
    // If not, this gives a null handle. This is only valid when the mesh is locked.
    Edge vertices_to_edge(Vertex a, Vertex b);

    void printout();
    
    // Element iterators.
    // usage:
    //     for (auto face : mesh.faces()) { ... }
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
    ElementContainer<Halfedge> halfedges() {
        return ElementContainer<Halfedge>(this, &halfedge_pool);
    }
    ElementContainer<Edge> edges() {
        assert(locked());
        return ElementContainer<Edge>(this, &edge_pool);
    }
    ElementContainer<Face> faces() {
        return ElementContainer<Face>(this, &face_pool);
    }

private:
    // Mesh element storage.
    // An ElementPool only keeps a small amount of data on available element IDs,
    // and references to attachments.
    ElementPool vertex_pool;
    ElementPool halfedge_pool;
    ElementPool edge_pool;
    ElementPool face_pool;
    // Incidence information is stored as a collection of functions of the elements (called "attachments").
    // These are attached to the relevant ElementPools.
    VertexAttachment<VertexIncidenceData> vertex_incidence_data;
    HalfedgeAttachment<HalfedgeIncidenceData> halfedge_incidence_data;
    EdgeAttachment<EdgeIncidenceData> edge_incidence_data;
    FaceAttachment<FaceIncidenceData> face_incidence_data;

    VertexAttachment<uint8_t> vertex_on_boundary; // Cache boundary-ness of vertices when the mesh is locked.

    // Map from pairs of vertex indices to halfedge indices.
    // get_halfedge is used to test if there is already a halfedge between two vertices,
    // and when setting up twin incidence relations.
    std::map<std::pair<ElementIndex, ElementIndex>, ElementIndex> halfedge_map; //vertices to halfedge.
    Halfedge get_halfedge(Vertex u, Vertex v);


    // Private topology data.
    std::vector<Halfedge> m_boundary_loops;
    std::vector<Face> m_connected_components;
    bool m_locked;
    int m_num_interior_vertices; // Only valid when locked.
    int m_num_interior_edges;    // Only valid when locked.

    
    template <typename T>
    friend class ElementAttachment;
    template <typename T>
    friend class VertexAttachment;
    template <typename T>
    friend class HalfedgeAttachment;
    template <typename T>
    friend class EdgeAttachment;
    template <typename T>
    friend class FaceAttachment;

    friend class Vertex;
    friend class Halfedge;
    friend class Edge;
    friend class Face;

    friend class ElementIterator<Vertex>;
    friend class ElementIterator<Halfedge>;
    friend class ElementIterator<Edge>;
    friend class ElementIterator<Face>;
};


#include "mesh_processing/surface_mesh/surface_mesh.ipp"



#endif // SURFACE_MESH_H
