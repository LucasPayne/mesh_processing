#include "mesh_processing/mesh_processing.h"
#include "tetgen.h"



TetgenMesh::TetgenMesh(SurfaceGeometry &geom)
{
    assert(geom.mesh.locked() && geom.mesh.closed());
    
    // Create tetgen input PLC (piecewise linear complex).
    //------------------------------------------------------------
    tetgenio in;

    in.firstnumber = 0;
    in.mesh_dim = 3;

    // Points.
    in.numberofpoints = geom.mesh.num_vertices();
    auto points = std::vector<REAL>(3 * geom.mesh.num_vertices());
    size_t vertex_index = 0;
    VertexAttachment<int> contiguous_vertex_indices(geom.mesh); // SurfaceMesh gives no guarantee that all vertex indices are used, so renumber them.
    for (auto v : geom.mesh.vertices()) {
        vec_t pos = geom.position[v];
        points[3*vertex_index+0] = pos.x();
        points[3*vertex_index+1] = pos.y();
        points[3*vertex_index+2] = pos.z();
        contiguous_vertex_indices[v] = vertex_index;
        vertex_index += 1;
    }
    in.pointlist = &points[0];
    in.pointattributelist = nullptr;

    // Facets.
    in.numberoffacets = geom.mesh.num_faces();
    auto facets = std::vector<tetgenio::facet>(geom.mesh.num_faces());
    auto facetmarkers = std::vector<int>(geom.mesh.num_faces());
    // There is one polygon per facet, so store them all in one array for convenience.
    auto polygons = std::vector<tetgenio::polygon>(geom.mesh.num_faces());
    

    in.facetlist = &facets[0];
    in.facetmarkerlist = &facetmarkers[0];


    // Store all polygon indices contiguously.
    size_t num_vertex_indices = 0;
    for (auto face : geom.mesh.faces()) {
        num_vertex_indices += face.num_vertices();
    }
    auto vertex_indices = std::vector<int>(num_vertex_indices);


    size_t facet_index = 0;
    size_t vertex_indices_offset = 0;
    for (auto face : geom.mesh.faces()) {
        tetgenio::facet *f = &facets[facet_index];
        f->numberofpolygons = 1;
        f->numberofholes = 0;
        f->holelist = nullptr;
        
        // The polygonlist contains one polygon.
        f->polygonlist = &polygons[facet_index]; // Point to a length-1 subarray.
        tetgenio::polygon *p = &f->polygonlist[0];
        // Set the vertexlist to a subarray of vertex_indices.
        p->numberofvertices = face.num_vertices();
        p->vertexlist = &vertex_indices[vertex_indices_offset];
        // Traverse the face to get vertices, and look up their contiguous index to store in the vertexlist.
        auto start = face.halfedge();
        auto he = start;
        size_t vertex_index = 0;
        do {
            p->vertexlist[vertex_index] = contiguous_vertex_indices[he.vertex()];
            vertex_index += 1;
        } while ((he = he.next()) != start);

        vertex_indices_offset += p->numberofvertices;
        facet_index += 1;
    }

    // Retrieve tetgen output.
    //------------------------------------------------------------
    tetgenio out;
    tetrahedralize("pq1.414a0.1", &in, &out);
    out.save_nodes("barout");
    out.save_elements("barout");
    out.save_faces("barout");
}
