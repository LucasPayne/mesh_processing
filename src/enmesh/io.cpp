#include <iomanip>
#include "mesh_processing/mesh_processing.h"
namespace Enmesh {


void save_geometry(SurfaceGeometry &geom, const std::string &filename)
{
    assert(geom.mesh.is_triangular());

    std::ofstream out;
    out.open(filename, std::ios::out | std::ios::trunc);
    assert(out);

    out << "OFF\n";
    out << std::to_string(geom.mesh.num_vertices()) << " " << std::to_string(geom.mesh.num_faces()) << " 0\n";
    
    // Vertex indices in the ElementPool sense are not necessarily contiguous.
    // So, a temporary attachment is used to give contiguous indices to the vertices.
    auto vertex_indices = VertexAttachment<uint32_t>(geom.mesh);
    int index = 0;
    for (auto vertex : geom.mesh.vertices()) {
        vertex_indices[vertex] = index; // Give the vertices contiguous indices.
        vec_t position = geom.position[vertex];
        out << std::setprecision(5) << "" << position.x() << " " << position.y() << " " << position.z() << "\n";
        index ++;
    }
    for (auto face : geom.mesh.faces()) {
        std::cout << "3 "; //---triangular mesh
        auto start = face.halfedge();
        auto he = start;
        do {
            uint32_t vertex_index = vertex_indices[he.vertex()];
            out << std::to_string(vertex_index) << " ";
            he = he.next();
        } while (start != he);
        out << "\n";
    }

    out.close();
}


} // namespace Enmesh
