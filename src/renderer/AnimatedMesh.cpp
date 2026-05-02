#include "AnimatedMesh.h"
#include "AnimatedMesh.h"
#include "AnimatedMesh.h"
#include "AnimatedMesh.h"
#include "renderer/AnimatedMesh.h"
#include "renderer/Shader.h"
#include "renderer/ShaderManager.h"
#include "core/Log.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtx/quaternion.hpp>

AnimatedMesh::AnimatedMesh(const std::string& path) {
    shaderHandle = ShaderManager::load("animated_mesh.vert", "animated_mesh.frag");

    m_scene = m_importer.ReadFile(path,
            aiProcess_CalcTangentSpace|
            aiProcess_Triangulate           |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType           |
            aiProcess_GenNormals);

    if (!m_scene) {
        Log::error(m_importer.GetErrorString());
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
    std::vector<glm::mat4> transforms;
    getBoneTransforms(transforms);
    for (int i = 0 ; i < m_bonesInfo.size(); ++i) {
        shader.set(("gBones["+std::to_string(i)+"]").c_str(), transforms[i]);
    }
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

glm::mat4 AnimatedMesh::calculateInterpolatedRotation(const double& animationTicks, const aiNodeAnim* animationNode) {
    if (animationNode->mNumRotationKeys == 1) {
        glm::quat out = glm::make_quat(&animationNode->mRotationKeys[0].mValue.x);
        out = normalize(out);
        return glm::toMat4(out);
    }
    int rotation_index = 0;
    for (unsigned int j = 0; j < animationNode->mNumRotationKeys - 1; ++j) {
        double t = animationNode->mRotationKeys[j + 1].mTime;
        if (animationTicks < t) {
            rotation_index = j;
            break;
        }
    }
    int next_rotation_index = rotation_index + 1;
    assert(next_rotation_index < animationNode->mNumRotationKeys);
    double t1 = animationNode->mRotationKeys[rotation_index].mTime;
    double t2 = animationNode->mRotationKeys[next_rotation_index].mTime;
    double delta_time = t2 - t1;
    double factor = (animationTicks-t1) / delta_time;
    const aiQuaternion& start = animationNode->mRotationKeys[rotation_index].mValue;
    const aiQuaternion& end = animationNode->mRotationKeys[next_rotation_index].mValue;
    aiQuaternion out;
    aiQuaternion::Interpolate(out,start,end,static_cast<float>(factor));
    out.Normalize();
    return glm::toMat4(glm::quat(out.w, out.x, out.y, out.z));
}
glm::mat4 AnimatedMesh::calculateInterpolatedPosition(const double& animationTicks, const aiNodeAnim* animationNode) {
    if (animationNode->mNumPositionKeys == 1) {
        glm::vec3 out = glm::make_vec3(&animationNode->mPositionKeys[0].mValue.x);
        return glm::translate(glm::mat4(1.0f), out);
    }
    int position_index = 0;
    for (unsigned int j = 0; j < animationNode->mNumPositionKeys - 1; ++j) {
        double t = animationNode->mPositionKeys[j + 1].mTime;
        if (animationTicks < t) {
            position_index = j;
            break;
        }
    }
    int next_position_index = position_index + 1;
    assert(next_position_index < animationNode->mNumPositionKeys);
    double t1 = animationNode->mPositionKeys[position_index].mTime;
    double t2 = animationNode->mPositionKeys[next_position_index].mTime;
    double delta_time = t2 - t1;
    double factor = (animationTicks-t1) / delta_time;
    const aiVector3D& start = animationNode->mPositionKeys[position_index].mValue;
    const aiVector3D& end = animationNode->mPositionKeys[next_position_index].mValue;
    aiVector3D delta = end - start;
    glm::vec3 out = glm::make_vec3(&start.x) + ((float)factor)*glm::make_vec3(&delta.x);
    return glm::translate(glm::mat4(1.0f), out);
}
glm::mat4 AnimatedMesh::calculateInterpolatedScale(const double& animationTicks, const aiNodeAnim* animationNode) {

    if (animationNode->mNumScalingKeys == 1) {
        glm::vec3 out = glm::make_vec3(&animationNode->mScalingKeys[0].mValue.x);
        return glm::scale(glm::mat4(1.0f), out);
    }
    int scale_index = 0;
    for (unsigned int j = 0; j < animationNode->mNumScalingKeys - 1; ++j) {
        double t = animationNode->mScalingKeys[j + 1].mTime;
        if (animationTicks < t) {
            scale_index = j;
            break;
        }
    }
    int next_scale_index = scale_index + 1;
    assert(next_scale_index < animationNode->mNumScalingKeys);
    double t1 = animationNode->mScalingKeys[scale_index].mTime;
    double t2 = animationNode->mScalingKeys[next_scale_index].mTime;
    double delta_time = t2 - t1;
    double factor = (animationTicks-t1) / delta_time;
    const aiVector3D& start = animationNode->mScalingKeys[scale_index].mValue;
    const aiVector3D& end = animationNode->mScalingKeys[next_scale_index].mValue;
    aiVector3D delta = end - start;
    glm::vec3 out = glm::make_vec3(&start.x) + ((float)factor)*glm::make_vec3(&delta.x);
    return glm::scale(glm::mat4(1.0f), out);
}

void AnimatedMesh::readNodeHierarchy(const double& animationTicks, const aiNode *node, const glm::mat4 &parentTransformation) const {
    glm::mat4 node_transformation = aiMat4ToGlmMat4(node->mTransformation);
    std::string nodeName = node->mName.data;

    const aiAnimation* animation = m_scene->mAnimations[0];
    const aiNodeAnim* animation_node = nullptr;
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        const aiNodeAnim* animation_node_i = animation->mChannels[i];
        if (std::string(animation_node_i->mNodeName.data) == nodeName) {
            animation_node = animation_node_i;
            break;
        }
    }

    if (animation_node) {
        glm::mat4 translation = calculateInterpolatedPosition(animationTicks, animation_node);
        glm::mat4 rotation = calculateInterpolatedRotation(animationTicks, animation_node);
        glm::mat4 scale = calculateInterpolatedScale(animationTicks, animation_node);
        node_transformation=translation*rotation*scale;
    }

    glm::mat4 globalTransformation = parentTransformation*node_transformation;

    if (m_boneNameToIndexMap.count(nodeName) != 0) {
        auto idx = m_boneNameToIndexMap[nodeName];
        auto& bone = m_bonesInfo[idx];
        bone.finalTransformationMatrix = m_globalInverseTransform*globalTransformation*bone.offsetMatrix;
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        readNodeHierarchy(animationTicks,node->mChildren[i], globalTransformation);
    }
}

void AnimatedMesh::getBoneTransforms(std::vector<glm::mat4> &transforms) const {
    if (m_scene->HasAnimations()) {
        double time = timer.getCurrentValue();
        double ticks_per_second = m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f;
        double ticks = fmod(time*ticks_per_second,m_scene->mAnimations[0]->mDuration);
        transforms.resize(m_bonesInfo.size());

        auto identity = glm::identity<glm::mat4>();

        readNodeHierarchy(ticks, m_scene->mRootNode, identity);
        for (unsigned int i = 0; i < m_bonesInfo.size(); i++) {
            transforms[i] = m_bonesInfo[i].finalTransformationMatrix;
        }
    }
    else {
        transforms.resize(m_bonesInfo.size());

        auto identity = glm::identity<glm::mat4>();

        readNodeHierarchy(0, m_scene->mRootNode, identity);
        for (unsigned int i = 0; i < m_bonesInfo.size(); i++) {
            transforms[i] = m_bonesInfo[i].finalTransformationMatrix;
        }
    }


}

void AnimatedMesh::parseMeshes() {
    m_globalInverseTransform = glm::inverse(aiMat4ToGlmMat4(m_scene->mRootNode->mTransformation));
    unsigned int baseVertex = 0;
    Log::info("scene " + (!std::string(m_scene->mName.data).empty() ? std::string(m_scene->mName.data) : "Untitled") + " : ");
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
                if (boneID == m_bonesInfo.size()) {
                    BoneInfo bi{aiMat4ToGlmMat4(bone->mOffsetMatrix)};
                    m_bonesInfo.push_back(bi);
                }
                for (unsigned int w = 0; w < bone->mNumWeights; w++) {
                    unsigned int globalVertexID = baseVertex + bone->mWeights[w].mVertexId;
                    m_vertices[globalVertexID].addBoneData(boneID, bone->mWeights[w].mWeight);
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
                m_indices.push_back(baseVertex + face.mIndices[0]);
                m_indices.push_back(baseVertex + face.mIndices[1]);
                m_indices.push_back(baseVertex + face.mIndices[2]);
            }
        }
        else {
            Log::info(" - no faces");
        }
        baseVertex += mesh->mNumVertices;
    }

    m_indexCount = static_cast<GLsizei>(m_indices.size());
}

void AnimatedMesh::populateBuffers() {

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

    struct alignas(16) VertexBoneData {
        unsigned int ids[MAX_NUM_BONES_PER_VERTEX];
        float weights[MAX_NUM_BONES_PER_VERTEX];
    };

    auto allBones = std::vector<VertexBoneData>(m_vertices.size(),VertexBoneData());

    for (int i = 0 ; i < m_vertices.size(); i++) {
        // fill boneIDs
        auto bone = VertexBoneData{};
        for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; j++) {
            bone.ids[j] = m_vertices[i].bonesIDs[j];
            bone.weights[j] = m_vertices[i].weights[j];
        }
        allBones[i] = bone;
    }

    glCreateBuffers(1, &bones_data_ssbo);
    glNamedBufferData(bones_data_ssbo, allBones.size() * sizeof(VertexBoneData), allBones.data(), GL_STATIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, bones_data_ssbo);

    timer.start();
}

void AnimatedMesh::setTimer(const double& minTime, const double& maxTime) {
    this->timer = Timer(minTime,maxTime);
}
