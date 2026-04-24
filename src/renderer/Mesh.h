/**
 * @file Mesh.h
 * @brief GPU mesh: VAO, VBO, and EBO managed as a single RAII object.
 */
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

/**
 * @brief A single vertex storing position and normal in world space.
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

/**
 * @brief Owns the GPU resources (VAO/VBO/EBO) for a triangle mesh.
 *
 * Uploads geometry once at construction via the OpenGL 4.5 DSA API.
 * Non-copyable; GPU resources are released in the destructor.
 */
class Mesh {
public:
    /**
     * @brief Uploads vertex and index data to the GPU.
     * @param vertices Interleaved position + normal data.
     * @param indices  Triangle index list (groups of 3).
     */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    ~Mesh();

    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    /**
     * @brief Binds the VAO and issues a draw call.
     */
    void draw() const;

    /// @brief Returns the number of indices in the mesh.
    GLsizei indexCount() const;

private:
    GLuint  m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLsizei m_indexCount = 0;
};
