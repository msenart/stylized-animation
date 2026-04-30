#pragma once
#include <cstdint>
#include <string>

class Shader;

using ShaderHandle = uint32_t;

/**
 * @brief Static registry for compiled GLSL shader programs.
 *
 * Deduplicates by source file paths: calling load() twice with the same paths
 * returns the same handle without recompilation. All registered programs can be
 * rebuilt from disk at once via reloadAll(). The old program is kept alive if
 * recompilation fails. Call shutdown() before the OpenGL context is destroyed.
 */
class ShaderManager {
public:
    ShaderManager()                              = delete;
    ShaderManager(const ShaderManager&)          = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    /**
     * @brief Compiles and registers a rasterization shader program.
     *
     * If a program with identical file paths has already been loaded, the
     * existing handle is returned without recompilation.
     *
     * @param vert  Vertex shader filename (relative to ./shaders/).
     * @param frag  Fragment shader filename (required).
     * @param geom  Geometry shader filename (empty = skip).
     * @param tesc  Tessellation control shader filename (empty = skip).
     * @param tese  Tessellation evaluation shader filename (empty = skip).
     * @return Stable handle valid until shutdown().
     * @throws std::runtime_error if compilation or linking fails.
     */
    static ShaderHandle load(const std::string& vert,
                             const std::string& frag,
                             const std::string& geom = {},
                             const std::string& tesc = {},
                             const std::string& tese = {});

    /**
     * @brief Returns the compiled program for a given handle.
     * @throws std::runtime_error if the handle is unknown.
     */
    static const Shader& get(ShaderHandle handle);

    /**
     * @brief Reloads every registered program from disk.
     *
     * On compile/link failure the old program is kept and an error is logged.
     * On success logs the number of programs reloaded.
     */
    static void reloadAll();

    /// @brief ImGui panel with a "Reload All" button and per-shader status.
    static void drawUI();

    /// @brief Destroys all GL programs. Must be called before the GL context is torn down.
    static void shutdown();
};
