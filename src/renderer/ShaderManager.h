#pragma once
#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <sstream>
class Shader;

using ShaderHandle = uint32_t;

/**
 * Contains all the information about a shader program : what path to shader it uses, and the shader variants (#define macros)
 */
struct ShaderKey {
    std::string vert, frag, geom, tesc, tese;
    std::set<std::string> defines = {};

    ShaderKey() = default;

    ShaderKey(std::string vert,
              std::string frag,
              std::string geom = "",
              std::string tesc = "",
              std::string tese = "",
              const std::set<std::string> &defines_v = {})
        : vert(std::move(vert)), frag(std::move(frag)), geom(std::move(geom)),
          tesc(std::move(tesc)), tese(std::move(tese)), defines(defines_v) {}

    [[nodiscard]] std::string getDescription() const {
        std::stringstream ss;

        // On liste les macros (ex: [TOON_SHADING][CONTOURS])
        for (const auto& define : defines) {
            ss << "[" << define << "]";
        }

        ss << "/";
        if (!vert.empty()) ss << vert << "/";
        if (!geom.empty()) ss << geom << "/";
        if (!tesc.empty()) ss << tesc << "/";
        if (!tese.empty()) ss << tese << "/";
        if (!frag.empty()) ss << frag << "/";

        return ss.str();
    }

    bool operator==(const ShaderKey& o) const {
        // ATTENTION : On utilise *defines et *o.defines pour comparer le CONTENU des sets
        return vert == o.vert && frag == o.frag && geom == o.geom
            && tesc == o.tesc && tese == o.tese && defines == o.defines;
    }
};

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
     * @param defines All the macro modifiers to compile a specific shader.
     * @return Stable handle valid until shutdown().
     * @throws std::runtime_error if compilation or linking fails.
     */
    static ShaderHandle load(std::set<std::string>& defines,
                            const std::string& vert,
                             const std::string& frag,
                             const std::string& geom = {},
                             const std::string& tesc = {},
                             const std::string& tese = {});

    /**
     *
     * @param key shader key to load the shader (cf. ShaderKey)
     * @return shader handle, stable until shutdown().
     */
    static ShaderHandle load(ShaderKey& key);

    /**
     * @brief Returns the compiled program for a given handle.
     * @throws std::runtime_error if the handle is unknown.
     */
    static const Shader& get(ShaderHandle handle);

    static ShaderHandle getShaderHandleWithKey(const ShaderKey& shader_key);
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
