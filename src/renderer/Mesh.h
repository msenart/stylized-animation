#pragma once
#include <glm/glm.hpp>
#include "renderer/ShaderManager.h"

class Shader;

struct RenderContext {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 viewPos;
    glm::vec3 objectColor;
};

class Mesh {
public:
    ShaderHandle shaderHandle = 0;

    virtual void draw() const = 0;
    virtual void uploadUniforms(const Shader& shader, const RenderContext& ctx) const;
    virtual ~Mesh() = default;

    Mesh()                         = default;
    Mesh(const Mesh&)              = delete;
    Mesh& operator=(const Mesh&)   = delete;
};