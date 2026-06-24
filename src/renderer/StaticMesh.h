#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <map>

#include "RenderPipeline.h"
#include "renderer/Mesh.h"
#include "scene/Scene.h"

enum class PassTag;

struct StaticVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class StaticMesh : public Mesh {
public:
    StaticMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t>& indices);
    ~StaticMesh() override;

    //StaticMesh(const StaticMesh&)            = delete;
    //StaticMesh& operator=(const StaticMesh&) = delete;

    void draw() const override;
    void uploadUniforms(const Shader& shader, const RenderContext& ctx) const override;

    GLsizei indexCount() const;

    //TODO! check if this shaders work
    [[nodiscard]] const std::map<PassTag, ShaderKey> shaderKeysMap() const override{
        std::map<PassTag, ShaderKey> key_map ={
            {PassTag::Hybrid , ShaderKey{"blinn_phong.vert","hybrid.frag"}},
        };
        return key_map;
    }
private:
    // const static std::map<MeshPassTag,ShaderKey> SHADER_KEYS_MAP; Must define SHADER_KEYS_MAP ! Don't forget !
    GLuint  m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLsizei m_indexCount = 0;
};