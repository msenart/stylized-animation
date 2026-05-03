#pragma once
#include <vector>
#include <array>
#include <map>
#include <string>
#include <cstdint>
#include <cassert>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/Timer.h"
#include "assimp/Importer.hpp"
#include "renderer/Mesh.h"

static constexpr unsigned int MAX_NUM_BONES_PER_VERTEX = 16;
static constexpr unsigned int MAX_NUM_BONES = 128;

static constexpr unsigned int POSITION_LOCATION = 0;
static constexpr unsigned int NORMAL_LOCATION = 1;
static constexpr unsigned int UV_LOCATION = 2;
static constexpr unsigned int BONE_ID_LOCATION = 3;
static constexpr unsigned int WEIGHTS_LOCATION = 4;

struct AnimatedVertex {
    glm::vec3 position{0};
    glm::vec3 normal{0};
    std::array<unsigned int,MAX_NUM_BONES_PER_VERTEX> bonesIDs = {0,0,0,0};
    std::array<float, MAX_NUM_BONES_PER_VERTEX> weights = {0,0,0,0};
    void addBoneData(unsigned int boneID, float weight) {
        for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
            if (weights[i] == 0.0f) {
                bonesIDs[i] = boneID;
                weights[i]  = weight;
                return;
            }
        }
        assert(false);
    }
};

struct BoneInfo {
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransformationMatrix;

    explicit BoneInfo(const glm::mat4& offsetMatrix) : offsetMatrix(offsetMatrix), finalTransformationMatrix(glm::mat4(0)) {

    }
};

inline glm::mat4 aiMat4ToGlmMat4(const aiMatrix4x4& m) {
    glm::mat4 glmMat = glm::transpose(glm::make_mat4(&m.a1));
    return glmMat;
}

class AnimatedMesh : public Mesh {
public:
    explicit AnimatedMesh(const std::string& path);
    ~AnimatedMesh() override;

    AnimatedMesh(const AnimatedMesh&)            = delete;
    AnimatedMesh& operator=(const AnimatedMesh&) = delete;

    void draw() const override;
    void uploadUniforms(const Shader& shader, const RenderContext& ctx) const override;
    Timer* getTimer() {
        return &timer;
    }

private:
    const aiScene* m_scene = nullptr;
    Assimp::Importer m_importer;
    std::vector<AnimatedVertex> m_vertices = {};
    std::vector<uint32_t> m_indices = {};
    GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLuint bones_data_ssbo = 0;
    GLsizei m_indexCount = 0;
    Timer timer = Timer(0,100);
    glm::mat4 m_globalInverseTransform;

    mutable std::map<std::string, int> m_boneNameToIndexMap;
    mutable std::vector<BoneInfo> m_bonesInfo;

    int  getBoneID(const aiBone* bone);

    static glm::mat4 calculateInterpolatedPosition(const double& animationTicks, const aiNodeAnim* animationNode);

    static glm::mat4 calculateInterpolatedRotation(const double& animationTicks, const aiNodeAnim* animationNode);

    static glm::mat4 calculateInterpolatedScale(const double& animationTicks, const aiNodeAnim* animationNode);

    void readNodeHierarchy(const double& animationTicks,const aiNode* node, const glm::mat4& parentTransformation) const;
    void getBoneTransforms(std::vector<glm::mat4>& transforms) const;
    void parseMeshes();
    void populateBuffers();
    void setTimer(const double& minTime, const double& maxTime);
};