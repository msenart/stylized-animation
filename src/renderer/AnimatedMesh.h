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
#include "renderer/Mesh.h"

static constexpr unsigned int MAX_NUM_BONES_PER_VERTEX = 4;
static constexpr unsigned int MAX_NUM_BONES = 1024;

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

struct Bone {};

class AnimatedMesh : public Mesh {
public:
    explicit AnimatedMesh(const std::string& path);
    ~AnimatedMesh() override;

    AnimatedMesh(const AnimatedMesh&)            = delete;
    AnimatedMesh& operator=(const AnimatedMesh&) = delete;

    void draw() const override;
    void uploadUniforms(const Shader& shader, const RenderContext& ctx) const override;

private:
    const aiScene* m_scene = nullptr;
    std::vector<AnimatedVertex> m_vertices = {};
    std::vector<uint32_t> m_indices = {};
    GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLsizei m_indexCount = 0;

    std::map<std::string, int> m_boneNameToIndexMap;
    std::vector<Bone> m_bones;

    int  getBoneID(const aiBone* bone);
    void parseMeshes();
    void populateBuffers();
};