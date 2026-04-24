/**
 * @file Shader.h
 * @brief GLSL shader program wrapper with uniform upload helpers.
 */
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

/**
 * @brief Compiles and links a vertex + fragment GLSL shader program.
 *
 * Compilation errors throw std::runtime_error with the GLSL error log.
 * Non-copyable; the underlying program is deleted in the destructor.
 */
class Shader {
public:
    /**
     * @brief Compiles and links the shader program.
     * @param vertSrc GLSL source for the vertex stage.
     * @param fragSrc GLSL source for the fragment stage.
     * @throws std::runtime_error on compile or link failure.
     */
    Shader(const char* vertSrc, const char* fragSrc);
    ~Shader();

    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    /// @brief Binds this program for subsequent draw calls.
    void bind()   const;
    /// @brief Unbinds any active shader program.
    void unbind() const;

    /// @brief Uploads a 4×4 matrix uniform. @param name GLSL uniform name. @param v The matrix.
    void setMat4 (const char* name, const glm::mat4& v) const;
    /// @brief Uploads a vec3 uniform.        @param name GLSL uniform name. @param v The vector.
    void setVec3 (const char* name, const glm::vec3& v) const;
    /// @brief Uploads a float uniform.       @param name GLSL uniform name. @param v The value.
    void setFloat(const char* name, float v)             const;

private:
    GLuint m_id = 0;

    GLint loc(const char* name) const;

    /**
     * @brief Compiles a single shader stage.
     * @param type   GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
     * @param src    GLSL source string.
     * @return       The compiled shader object ID.
     * @throws std::runtime_error on compile failure.
     */
    static GLuint compile(GLenum type, const char* src);
};
