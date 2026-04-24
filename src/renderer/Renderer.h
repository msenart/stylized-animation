/**
 * @file Renderer.h
 * @brief Forward rendering pass with a Blinn-Phong shader.
 */
#pragma once
#include "renderer/Shader.h"
#include "scene/Scene.h"
#include "core/AssetManager.h"

/**
 * @brief Issues OpenGL draw calls for every Object in a Scene.
 *
 * Owns a compiled shader program loaded from shaders/blinn_phong.{vert,frag}.
 * The Renderer reads from the Scene and AssetManager; it never mutates them.
 */
class Renderer {
public:
    /**
     * @brief Loads shaders from disk and enables depth testing.
     * @throws std::runtime_error if a shader file cannot be opened or compilation fails.
     */
    Renderer();

    /**
     * @brief Clears the framebuffer and draws every object in the scene.
     *
     * Uploads camera matrices and the first light's uniforms once per frame,
     * then iterates over all objects calling Mesh::draw().
     *
     * @param scene   Read-only scene providing camera, lights, and objects.
     * @param assets  Read-only asset manager resolving MeshHandles.
     * @param aspect  Viewport width / height for the projection matrix.
     */
    void render(const Scene& scene, const AssetManager& assets, float aspect);

    /// @brief Returns the number of draw calls issued during the last render().
    int drawCalls() const;

    /// @brief Reloads the shader program from disk. Logs an error and keeps the
    ///        current program alive if the new sources fail to compile.
    void reloadShaders();

private:
    Shader m_shader;
    int    m_drawCalls = 0;
};
