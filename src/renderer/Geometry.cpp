#include "renderer/Geometry.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "core/Log.h"

namespace Geometry {

    MeshData makeCube() {
        MeshData d;
        d.vertices = {
            // Front  (+Z)
            {{-0.5f, -0.5f,  0.5f}, { 0,  0,  1}},
            {{ 0.5f, -0.5f,  0.5f}, { 0,  0,  1}},
            {{ 0.5f,  0.5f,  0.5f}, { 0,  0,  1}},
            {{-0.5f,  0.5f,  0.5f}, { 0,  0,  1}},
            // Back   (-Z)
            {{ 0.5f, -0.5f, -0.5f}, { 0,  0, -1}},
            {{-0.5f, -0.5f, -0.5f}, { 0,  0, -1}},
            {{-0.5f,  0.5f, -0.5f}, { 0,  0, -1}},
            {{ 0.5f,  0.5f, -0.5f}, { 0,  0, -1}},
            // Left   (-X)
            {{-0.5f, -0.5f, -0.5f}, {-1,  0,  0}},
            {{-0.5f, -0.5f,  0.5f}, {-1,  0,  0}},
            {{-0.5f,  0.5f,  0.5f}, {-1,  0,  0}},
            {{-0.5f,  0.5f, -0.5f}, {-1,  0,  0}},
            // Right  (+X)
            {{ 0.5f, -0.5f,  0.5f}, { 1,  0,  0}},
            {{ 0.5f, -0.5f, -0.5f}, { 1,  0,  0}},
            {{ 0.5f,  0.5f, -0.5f}, { 1,  0,  0}},
            {{ 0.5f,  0.5f,  0.5f}, { 1,  0,  0}},
            // Bottom (-Y)
            {{-0.5f, -0.5f, -0.5f}, { 0, -1,  0}},
            {{ 0.5f, -0.5f, -0.5f}, { 0, -1,  0}},
            {{ 0.5f, -0.5f,  0.5f}, { 0, -1,  0}},
            {{-0.5f, -0.5f,  0.5f}, { 0, -1,  0}},
            // Top    (+Y)
            {{-0.5f,  0.5f,  0.5f}, { 0,  1,  0}},
            {{ 0.5f,  0.5f,  0.5f}, { 0,  1,  0}},
            {{ 0.5f,  0.5f, -0.5f}, { 0,  1,  0}},
            {{-0.5f,  0.5f, -0.5f}, { 0,  1,  0}},
        };
        d.indices = {
             0,  1,  2,  2,  3,  0,
             4,  5,  6,  6,  7,  4,
             8,  9, 10, 10, 11,  8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20,
        };
        return d;
    }

    std::vector<MeshData> loadMeshFromFile( const std::string& pFile) {

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile( pFile,
          aiProcess_CalcTangentSpace       |
          aiProcess_Triangulate            |
          aiProcess_JoinIdenticalVertices  |
          aiProcess_SortByPType |
          aiProcess_GenNormals);


        if (nullptr == scene) {
            Log::error( importer.GetErrorString());
            return std::vector<MeshData>{};
        }

        if (!scene->HasMeshes()) {
            Log::error( "no meshes found in path : " + pFile);
            return std::vector<MeshData>{};
        }
        std::vector<MeshData> meshes = {};
        for (int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[i];
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            // Extracting vertices
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                const aiVector3D position = mesh->mVertices[j];
                const aiVector3D normal = mesh->mNormals[j];
                auto vertex = Vertex{
                    glm::vec3(position.x, position.y, position.z),
                    glm::vec3(normal.x,normal.y,normal.z)};
                auto index = j;
                vertices.push_back(vertex);
            }
            // Extracting indices
            for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                const aiFace& face = mesh->mFaces[j];
                assert(face.mNumIndices == 3);
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
            MeshData meshData = {vertices, indices};
            meshes.push_back(meshData);
        }
        return meshes;
    }
} // namespace Geometry
