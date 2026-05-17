#pragma once
#include <map>
#include <glm/glm.hpp>
#include "renderer/ShaderManager.h"
#include "Renderer.h"
enum class PassTag;
class Shader;

class Mesh {
public:
    virtual const std::map<PassTag, ShaderKey> shaderKeysMap() const = 0;

    virtual void draw() const = 0;
    virtual void uploadUniforms(const Shader& shader, const RenderContext& ctx) const;
    virtual ~Mesh() = default;

    Mesh()                         = default;
    Mesh(const Mesh&)              = delete;
    Mesh& operator=(const Mesh&)   = delete;
};