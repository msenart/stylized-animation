#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include "renderer/Mesh.h"

struct StaticVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class StaticMesh : public Mesh {
public:
    StaticMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t>& indices);
    ~StaticMesh() override;

    StaticMesh(const StaticMesh&)            = delete;
    StaticMesh& operator=(const StaticMesh&) = delete;

    void draw() const override;
    void uploadUniforms(const Shader& shader, const RenderContext& ctx) const override;

    GLsizei indexCount() const;

private:
    GLuint  m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLsizei m_indexCount = 0;
};