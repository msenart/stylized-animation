#include "renderer/AnimatedMesh.h"
#include "renderer/Shader.h"
#include "renderer/ShaderManager.h"
#include "core/Log.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

AnimatedMesh::AnimatedMesh(const std::string& path) {
    shaderHandle = ShaderManager::load("animated_mesh.vert", "animated_mesh.frag");

    Assimp::Importer importer;
    m_scene = importer.ReadFile(path,
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType           |
        aiProcess_GenNormals);

    if (!m_scene) {
        Log::error(importer.GetErrorString());
        return;
    }
    if (!m_scene->HasMeshes()) {
        Log::error("no meshes found in path: " + path);
        return;
    }

    parseMeshes();
    populateBuffers();
}

AnimatedMesh::~AnimatedMesh() {
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void AnimatedMesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES,m_indexCount,GL_UNSIGNED_INT,nullptr);
}

void AnimatedMesh::uploadUniforms(const Shader& shader, const RenderContext& ctx) const {
    Mesh::uploadUniforms(shader,ctx);
}

int AnimatedMesh::getBoneID(const aiBone* bone) {
    std::string boneName(bone->mName.data);
    auto it = m_boneNameToIndexMap.find(boneName);
    if (it == m_boneNameToIndexMap.end()) {
        int id = static_cast<int>(m_boneNameToIndexMap.size());
        m_boneNameToIndexMap[boneName] = id;
        return id;
    }
    return it->second;
}

void AnimatedMesh::parseMeshes() {
    Log::info("scene " + (std::string(m_scene->mName.data).size() > 0 ? std::string(m_scene->mName.data) : "Untitled") + " : ");
    for (unsigned int i = 0; i < m_scene->mNumMeshes; i++) {
        const aiMesh* mesh = m_scene->mMeshes[i];
        Log::info("mesh " + std::string(mesh->mName.data) + " : ");
        if (mesh->HasPositions() && mesh->HasNormals()) {
            Log::info(" - has " + std::to_string(mesh->mNumVertices) + " vertices");
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                const aiVector3D& p = mesh->mVertices[j];
                const aiVector3D& n = mesh->mNormals[j];
                m_vertices.push_back(AnimatedVertex{
                    glm::vec3(p.x,p.y,p.z),
                    glm::vec3(n.x,n.y,n.z)
                });
            }
        }
        else {
            Log::info(" - no mesh found");
        }

        if (mesh->HasBones()) {
            Log::info(" - has " + std::to_string(mesh->mNumBones) + " bones");
            for (unsigned int b = 0; b < mesh->mNumBones; b++) {
                const aiBone* bone = mesh->mBones[b];
                int boneID = getBoneID(bone);
                for (unsigned int w = 0; w < bone->mNumWeights; w++) {
                    unsigned int vertexID = bone->mWeights[w].mVertexId;
                    m_vertices[vertexID].addBoneData(boneID, bone->mWeights[w].mWeight);
                }
                Log::info("   - bone " + std::to_string(boneID) + " influences " + std::to_string(mesh->mNumVertices) + " vertices");
            }
        }
        else {
            Log::info(" - no bones (cannot be animated !)");
        }

        if (mesh->HasFaces()) {
            Log::info(" - has " + std::to_string(mesh->mNumFaces) + " faces");
            for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                const aiFace& face = mesh->mFaces[j];
                assert(face.mNumIndices == 3);
                m_indices.push_back(face.mIndices[0]);
                m_indices.push_back(face.mIndices[1]);
                m_indices.push_back(face.mIndices[2]);
            }
        }
        else {
            Log::info(" - no faces");
        }
    }

    m_indexCount = static_cast<GLsizei>(m_indices.size());
}

void AnimatedMesh::populateBuffers() {
    static_assert(sizeof(AnimatedVertex) == (3 + 3 + MAX_NUM_BONES_PER_VERTEX + MAX_NUM_BONES_PER_VERTEX) * 4, "Struct size mismatch!");

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);
    glCreateBuffers(1, &m_ebo);

    glNamedBufferData(m_vbo,m_vertices.size()*sizeof(AnimatedVertex),m_vertices.data(),GL_STATIC_DRAW);
    glNamedBufferData(m_ebo,m_indices.size()* sizeof(uint32_t),m_indices.data(),GL_STATIC_DRAW);

    glVertexArrayVertexBuffer (m_vao,0,m_vbo,0,sizeof(AnimatedVertex));
    glVertexArrayElementBuffer(m_vao,m_ebo);

    glEnableVertexArrayAttrib (m_vao,POSITION_LOCATION);
    glVertexArrayAttribFormat (m_vao,POSITION_LOCATION,3,GL_FLOAT,GL_FALSE,offsetof(AnimatedVertex, position));
    glVertexArrayAttribBinding(m_vao,POSITION_LOCATION,0);

    glEnableVertexArrayAttrib (m_vao,NORMAL_LOCATION);
    glVertexArrayAttribFormat (m_vao,NORMAL_LOCATION,3,GL_FLOAT,GL_FALSE,offsetof(AnimatedVertex, normal));
    glVertexArrayAttribBinding(m_vao,NORMAL_LOCATION,0);

    glEnableVertexArrayAttrib (m_vao,BONE_ID_LOCATION);
    glVertexArrayAttribIFormat(m_vao,BONE_ID_LOCATION,MAX_NUM_BONES_PER_VERTEX,GL_UNSIGNED_INT,offsetof(AnimatedVertex,bonesIDs));
    glVertexArrayAttribBinding(m_vao,BONE_ID_LOCATION, 0);

    glEnableVertexArrayAttrib (m_vao,WEIGHTS_LOCATION);
    glVertexArrayAttribFormat (m_vao,WEIGHTS_LOCATION,MAX_NUM_BONES_PER_VERTEX,GL_FLOAT,GL_FALSE,offsetof(AnimatedVertex,weights));
    glVertexArrayAttribBinding(m_vao,WEIGHTS_LOCATION,0);
}