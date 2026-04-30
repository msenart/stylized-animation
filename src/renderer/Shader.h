/**
 * @file Shader.h
 * @brief GLSL shader program wrapper with uniform upload helpers.
 */
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <set>

/**
 * @brief Compiles and links a GLSL shader program.
 *
 * Supports the full rasterization pipeline (vertex, fragment, geometry,
 * tessellation control/evaluation) and the compute pipeline.
 * Compilation errors throw std::runtime_error with the GLSL error log.
 * Non-copyable, movable; the underlying program is deleted in the destructor.
 */
class Shader {
public:
    /**
     * @brief Compiles and links a rasterization shader program.
     * @param vertSrc  GLSL source for the vertex stage (required).
     * @param fragSrc  GLSL source for the fragment stage (required).
     * @param geomSrc  GLSL source for the geometry stage (optional, nullptr = skip).
     * @param tescSrc  GLSL source for the tessellation control stage (optional).
     * @param teseSrc  GLSL source for the tessellation evaluation stage (optional).
     * @throws std::runtime_error on compile or link failure.
     */
    Shader(const char* vertSrc, const char* fragSrc,
           const char* geomSrc = nullptr,
           const char* tescSrc = nullptr,
           const char* teseSrc = nullptr);

    /**
     * @brief Compiles and links a compute shader program.
     * @param compSrc  GLSL source for the compute stage.
     * @throws std::runtime_error on compile or link failure.
     */
    static Shader compute(const char* compSrc);

    /**
     * @brief Loads a rasterization shader program from .glsl files on disk.
     * @param vertPath  Path to the vertex shader file (required).
     * @param fragPath  Path to the fragment shader file (required).
     * @param geomPath  Path to the geometry shader file (empty string = skip).
     * @param tescPath  Path to the tessellation control shader file (empty = skip).
     * @param tesePath  Path to the tessellation evaluation shader file (empty = skip).
     * @throws std::runtime_error if a file cannot be opened or compilation fails.
     */
    static Shader fromFiles(const std::string& vertPath,
                            const std::string& fragPath,
                            const std::string& geomPath = {},
                            const std::string& tescPath = {},
                            const std::string& tesePath = {});

    /**
     * @brief Loads a compute shader program from a .glsl file on disk.
     * @param compPath  Path to the compute shader file.
     * @throws std::runtime_error if the file cannot be opened or compilation fails.
     */
    static Shader computeFile(const std::string& compPath);

    ~Shader();
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    /// Binds this program for subsequent draw calls.
    void bind()   const;
    /// Unbinds any active shader program.
    void unbind() const;

    // Uniform setters — the correct GL call is selected automatically by type.
    void set(const char* name, const glm::mat4& v) const;
    void set(const char* name, const glm::mat3& v) const;
    void set(const char* name, const glm::vec4& v) const;
    void set(const char* name, const glm::vec3& v) const;
    void set(const char* name, const glm::vec2& v) const;
    void set(const char* name, float v)             const;
    void set(const char* name, int v)               const;
    void set(const char* name, bool v)              const;

private:
    GLuint m_id = 0;

    explicit Shader(GLuint id) : m_id(id) {}

    GLint  loc(const char* name) const;
    static std::string readFile(const std::string& path);
    static GLuint compile(GLenum type, const char* src);
    static GLuint buildRasterProgram(const char* vertSrc, const char* fragSrc,
                                     const char* geomSrc, const char* tescSrc,
                                     const char* teseSrc);
    static GLuint buildComputeProgram(const char* compSrc);
    static std::string readRecursive(const std::string& path, std::set<std::string>& visited, int& counter);
};