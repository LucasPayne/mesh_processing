#include "mesh_processing/mesh_processing.h"
#include "mesh_processing/extensions/assimp_convert.h"


SurfaceGeometry assimp_to_surface_geometry(const std::string &filename)
{
    Assimp::Importer importer;
    auto flags = aiProcess_JoinIdenticalVertices;
    const aiScene *scene = importer.ReadFile(filename, flags);
    assert(scene);
    assert(scene->mNumMeshes > 0);

    printf("num meshes: %d\n", scene->mNumMeshes);

    SurfaceGeometry geom;
    for (int mesh_index = 0; mesh_index < scene->mNumMeshes; mesh_index++) {
        aiMesh *mesh = scene->mMeshes[mesh_index];
        printf("num vertices: %d\n", mesh->mNumVertices);
        printf("num faces: %d\n", mesh->mNumFaces);

        std::vector<Vertex> vertex_list(mesh->mNumVertices);
        for (int vertex_index = 0; vertex_index < mesh->mNumVertices; vertex_index++) {
            auto &position = mesh->mVertices[vertex_index];
            vertex_list[vertex_index] = geom.add_vertex(position.x, position.y, position.z);
        }
        // Get maximum number of vertices in a face.
        uint32_t max_face_vertices = 0;
        uint32_t min_face_vertices = std::numeric_limits<uint32_t>::max();
        for (int face_index = 0; face_index < mesh->mNumFaces; face_index++) {
            uint32_t n = mesh->mFaces[face_index].mNumIndices;
            max_face_vertices = std::max(max_face_vertices, n);
            min_face_vertices = std::min(min_face_vertices, n);
        }
        assert(max_face_vertices > 0 && min_face_vertices >= 3);

        std::vector<Vertex> face_vertices(max_face_vertices);
        for (int face_index = 0; face_index < mesh->mNumFaces; face_index++) {
            aiFace *face = &mesh->mFaces[face_index];
            for (uint32_t face_index_index = 0; face_index_index < face->mNumIndices; face_index_index++) {
                uint32_t index = face->mIndices[face_index_index];
                face_vertices[face_index_index] = vertex_list[index];
            }
            geom.mesh.add_face(&face_vertices[0], face->mNumIndices);
        }
    }
    
    return geom;
}


