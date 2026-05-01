#include "renderer/Geometry.h"
#include "core/Log.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>

namespace Geometry {

    StaticMeshData makeCube() {
        std::vector<StaticVertex> vertices = {
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
        std::vector<uint32_t> indices = {
             0,  1,  2,  2,  3,  0,
             4,  5,  6,  6,  7,  4,
             8,  9, 10, 10, 11,  8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20,
        };
        return StaticMeshData{vertices, indices};
    }

    std::vector<StaticMeshData> loadStaticMeshFromFile(const std::string& pFile) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(pFile,
            aiProcess_CalcTangentSpace      |
            aiProcess_Triangulate           |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType           |
            aiProcess_GenNormals);

        if (!scene) {
            Log::error(importer.GetErrorString());
            return {};
        }
        if (!scene->HasMeshes()) {
            Log::error("no meshes found in path: " + pFile);
            return {};
        }

        std::vector<StaticMeshData> meshes;
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[i];
            std::vector<StaticVertex> vertices;
            std::vector<uint32_t>     indices;

            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                const aiVector3D& p = mesh->mVertices[j];
                const aiVector3D& n = mesh->mNormals[j];
                vertices.push_back(StaticVertex{
                    glm::vec3(p.x, p.y, p.z),
                    glm::vec3(n.x, n.y, n.z)
                });
            }

            for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                const aiFace& face = mesh->mFaces[j];
                assert(face.mNumIndices == 3);
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            meshes.push_back(StaticMeshData{std::move(vertices), std::move(indices)});
        }
        return meshes;
    }

} // namespace Geometry
