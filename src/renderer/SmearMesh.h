#ifndef SMEARMESH_H_
#define SMEARMESH_H_
#include "renderer/Shader.h"
#include "renderer/ShaderManager.h"
#include "renderer/Types_renderer.h"
#include <vector>
#pragma once
#include <iostream>
#include <map>
#include <string>
#include <cassert>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Mesh.h"
#include "AnimatedMesh.h"

class SmearMesh : public AnimatedMesh {
  public:
    explicit SmearMesh(const std::string& path);
    ~SmearMesh() = default;
    SmearMesh(const SmearMesh&)            = delete;
    SmearMesh& operator=(const SmearMesh&) = delete;
    void uploadUniforms(const Shader &shader,
                        const RenderContext &ctx) const override;
    void drawDebugBones(const Shader& shader, const RenderContext& ctx) const;
    int getCurrentFrame() override;

  private:
    GLuint m_deltas_ssbo = 0;
    GLuint m_animation_bones_transforms_ssbo =
        0; // 2D matrix that holds all bone transforms throughout the whole
           // animation
    // DEBUG
    GLuint m_debugVao = 0;
    GLuint m_debugVbo = 0;

    int m_Fps = 12; // TODO get this information from main.cpp
    int m_totalFrames;
    std::vector<std::vector<int>> m_V_kr;
    std::vector<std::vector<int>> m_V_kt;

    const std::map<PassTag, ShaderKey> shaderKeysMap() const override {
      return std::map<PassTag, ShaderKey> {
      {PassTag::Hybrid, ShaderKey{"hybrid_smear.vert", "hybrid.frag"}}};
    }
    void computeVertexSets();
    void generateDeltasSSBO(const std::string& meshPath);
    std::string getDeltasFilePath(const std::string& meshPath) const;
    bool readFromDeltasFile(const std::string& path, std::vector<float>& outDeltas, int& outTotalFrames) const;
    void writeToDeltasFile(const std::string& path, const std::vector<float>& deltas, int totalFrames) const;
    void getFrameDeltas(std::vector<glm::mat4> currentTransforms, std::vector<glm::mat4> nextTransforms, std::vector<glm::mat4> frameSkinnedBones, std::vector<float> &outFrameDeltas);
    void createDeltasSSBO();
};

#endif // SMEARMESH_H_
