#include "mesh_processing/mesh_processing.h"

Eigen::SparseMatrix<double> cotan_laplacian(SurfaceGeometry &geom)
{
    assert(geom.mesh.locked());
    assert(geom.mesh.is_triangular());

    std::vector<Eigen::Triplet<double>> triplets;

    FaceAttachment<double> triangle_weighting_coefficient(geom.mesh);
    for (auto face : geom.mesh.faces()) {
        // Compute and cache 0.25/(area of triangle) for each triangle.
        vec_t x1 = geom.position[face.halfedge().vertex()];
        vec_t x2 = geom.position[face.halfedge().next().vertex()];
        vec_t x3 = geom.position[face.halfedge().next().next().vertex()];
        double tri_area = 0.5*(x2 - x1).cross(x3 - x1).length();
        triangle_weighting_coefficients[face] = 0.25/tri_area;
    }

    // Compute contiguous vertex indices.
    VertexAttachment<int> vertex_indices(geom.mesh);
    {
        int vindex = 0;
        for (auto v : geom.mesh.vertices()) {
            vertex_indices[v] = vindex ++;
        }
    }
    
    for (auto v : geom.mesh.vertices()) {
        auto start = v.halfedge();
        auto he = start;
        vec_t x1 = geom.position[v];
        int v1_index = vertex_indices[v];
        do {
            auto face = he.face();
            float c = triangle_weighting_coefficient[face]; // 0.25/(area of triangle).
            vec_t v2 = he.next().vertex();
            vec_t v3 = he.next().next().vertex();
            int v2_index = vertex_indices[v2];
            int v3_index = vertex_indices[v3];
            vec_t x2 = geom.position[v2];
            vec_t x3 = geom.position[v3];

            // Form a orthonormal basis for the triangle, centred at x1.
            vec_t e1 = (x2 - x1).normalized(); // e1: Direction x1->x2.
                                               // e2: Orthogonalized direction x1->x3.
            vec_t e2 = ((x3 - x1) - e1*(x3 - x1).dot(e1)).normalized(); // Gram-Schmidt to compute e2.

            // Compute inward-pointing normals.
            vec_t x13orth = -e2*(x3 - x1).dot(e1) + e1*(x3 - x1).dot(e2);
            vec_t x12orth = e2;
            vec_t x23orth = -e2*(x3 - x2).dot(e1) + e1*(x3 - x2).dot(e2);

            float d1 = x13orth.dot(x23orth);

            double d2 = (x2 - x1).length() * e2.dot(orth(x2 - x3));
            double f1w = c*(-d1 - d2);
            double f2w = c*d1;
            double f3w = c*d2;

            triplets.emplace_back(v1_index, v1_index, f1w);
            triplets.emplace_back(v1_index, v2_index, f2w);
            triplets.emplace_back(v1_index, v3_index, f3w); //---is setFromTriplets additive?
        } while ();
    }

    auto mat = Eigen::SparseMatrix<double>(geom.mesh.num_vertices(), geom.mesh.num_vertices());
    mat.setFromTriplets(triplets.begin(), triplets.end());
    return mat;
}
