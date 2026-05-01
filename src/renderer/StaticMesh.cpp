#include "renderer/StaticMesh.h"
#include "renderer/Shader.h"
#include "renderer/ShaderManager.h"

static constexpr unsigned int POSITION_LOCATION = 0;
static constexpr unsigned int NORMAL_LOCATION   = 1;

StaticMesh::StaticMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t>& indices)
    : m_indexCount(static_cast<GLsizei>(indices.size()))
{
    shaderHandle = ShaderManager::load("blinn_phong.vert", "blinn_phong.frag");

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);
    glCreateBuffers(1, &m_ebo);

    glNamedBufferData(m_vbo, vertices.size() * sizeof(StaticVertex), vertices.data(), GL_STATIC_DRAW);
    glNamedBufferData(m_ebo, indices.size()  * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer (m_vao, 0, m_vbo, 0, sizeof(StaticVertex));
    glVertexArrayElementBuffer(m_vao, m_ebo);

    glEnableVertexArrayAttrib (m_vao, POSITION_LOCATION);
    glVertexArrayAttribFormat (m_vao, POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(StaticVertex, position));
    glVertexArrayAttribBinding(m_vao, POSITION_LOCATION, 0);

    glEnableVertexArrayAttrib (m_vao, NORMAL_LOCATION);
    glVertexArrayAttribFormat (m_vao, NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, offsetof(StaticVertex, normal));
    glVertexArrayAttribBinding(m_vao, NORMAL_LOCATION, 0);
}

StaticMesh::~StaticMesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void StaticMesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}

void StaticMesh::uploadUniforms(const Shader& shader, const RenderContext& ctx) const {
    Mesh::uploadUniforms(shader, ctx);
    shader.set("objectColor", ctx.objectColor);
    shader.set("lightPos",    ctx.lightPos);
    shader.set("lightColor",  ctx.lightColor);
    shader.set("viewPos",     ctx.viewPos);
}

GLsizei StaticMesh::indexCount() const {
    return m_indexCount;
}