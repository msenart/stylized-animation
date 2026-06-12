#ifndef SMEARMESH_H_
#define SMEARMESH_H_
#include "renderer/Shader.h"
#include "renderer/ShaderManager.h"
#include "renderer/Types_renderer.h"
#include <vector>
#pragma once
#include <map>
#include <string>
#include <cassert>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "RenderPipeline.h"
#include "renderer/Mesh.h"
#include "AnimatedMesh.h"

class SmearMesh : public AnimatedMesh {
  public:
    explicit SmearMesh(const std::string& path);
    ~SmearMesh() = default;
    SmearMesh(const SmearMesh&)            = delete;
    SmearMesh& operator=(const SmearMesh&) = delete;
    void uploadUniforms(const Shader &shader,
                        const RenderContext &ctx) const override;

  private:
    GLuint deltas_ssbo = 0;

    const std::map<PassTag, ShaderKey> shaderKeysMap() const override {
      return std::map<PassTag, ShaderKey> {
      {PassTag::Hybrid, ShaderKey{"hybrid_smear.vert", "hybrid.frag"}}};
    }
    void generateDeltasSSBO();
    void getFrameDeltas(std::vector<float> &outFrameDeltas);
    void createDeltasSSBO();
    void getBonePosition();
};

#endif // SMEARMESH_H_
